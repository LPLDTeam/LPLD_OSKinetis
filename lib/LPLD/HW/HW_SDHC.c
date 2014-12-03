/**
 * @file HW_SDHC.c
 * @version 3.01[By LPLD]
 * @date 2013-10-21
 * @brief SDHC底层模块相关函数
 *
 * 更改建议:不建议修改
 *
 * 版权所有:北京拉普兰德电子技术有限公司
 * http://www.lpld.cn
 * mail:support@lpld.cn
 *
 * @par
 * 本代码由拉普兰德[LPLD]开发并维护，并向所有使用者开放源代码。
 * 开发者可以随意修使用或改源代码。但本段及以上注释应予以保留。
 * 不得更改或删除原版权所有者姓名，二次开发者可以加注二次版权所有者。
 * 但应在遵守此协议的基础上，开放源代码、不得出售代码本身。
 * 拉普兰德不负责由于使用本代码所带来的任何事故、法律责任或相关不良影响。
 * 拉普兰德无义务解释、说明本代码的具体原理、功能、实现方法。
 * 除非拉普兰德[LPLD]授权，开发者不得将本代码用于商业产品。 
 *
 * 版权说明:
 *  SDHC模块驱动程序摘取自飞思卡尔MQX底层驱动，部分功能由拉普兰德修改。
 *  HW_SDHC.h及HW_SDHC.c内的代码版权归飞思卡尔公司享有。
 * 
 * 3.01-2013-10-21 修复低容量SD卡无法初始化BUG
 */
#include "common.h"
#include "HW_SDHC.h"

//SD卡信息全局变量
SDCARD_STRUCT_PTR sdcard_ptr;

/*
 * LPLD_SDHC_InitGPIO
 * 初始化SDHC模块相关的GPIO引脚,并使能SDHC寄存器时钟
 * 
 * 参数:
 *    init--PCR寄存器掩码
 *
 * 输出:
 *    无
 */
static void LPLD_SDHC_InitGPIO(uint32 init)
{  
  PORTE->PCR[0] = init & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* SDHC.D1  */
  PORTE->PCR[1] = init & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* SDHC.D0  */
  PORTE->PCR[2] = init & (PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK);                                          /* SDHC.CLK */
  PORTE->PCR[3] = init & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* SDHC.CMD */
  PORTE->PCR[4] = init & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* SDHC.D3  */
  PORTE->PCR[5] = init & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* SDHC.D2  */
  
  SIM->SCGC3 |= SIM_SCGC3_SDHC_MASK; 
}


/*
 * LPLD_SDHC_SetBaudrate
 * 设置SDHC波特率
 * 
 * 参数:
 *    clock--模块输入时钟，即g_core_clock*1000，单位Hz
 *    baud--SDHC期望时钟频率，单位Hz
 *
 * 输出:
 *    无
 */
static void LPLD_SDHC_SetBaudrate(uint32 clock, uint32 baud)
{
  uint32 pres, div, min, minpres = 0x80, mindiv = 0x0F;
  int32  val;
  
  //找到相近的分频因子
  min = (uint32)-1;
  for (pres = 2; pres <= 256; pres <<= 1)
  {
    for (div = 1; div <= 16; div++)
    {
      val = pres * div * baud - clock;
      if (val >= 0)
      {
        if (min > val)
        {
          min = val;
          minpres = pres;
          mindiv = div;
        }
      }
    }
  }
  
  //禁止SDHC模块时钟
  SDHC->SYSCTL &= (~ SDHC_SYSCTL_SDCLKEN_MASK);
  
  //修改分频因子
  div = SDHC->SYSCTL & (~ (SDHC_SYSCTL_DTOCV_MASK | SDHC_SYSCTL_SDCLKFS_MASK | SDHC_SYSCTL_DVS_MASK));
  SDHC->SYSCTL = div | (SDHC_SYSCTL_DTOCV(0x0E) | SDHC_SYSCTL_SDCLKFS(minpres >> 1) | SDHC_SYSCTL_DVS(mindiv - 1));
  
  //等在时钟稳定
  while (0 == (SDHC->PRSSTAT & SDHC_PRSSTAT_SDSTB_MASK))
  {};
  
  //使能SDHC模块时钟
  SDHC->SYSCTL |= SDHC_SYSCTL_SDCLKEN_MASK;
  SDHC->IRQSTAT |= SDHC_IRQSTAT_DTOE_MASK;
}

/*
 * LPLD_SDHC_IsRunning
 * 获取SDHC模块运行状态
 * 
 * 参数:
 *    无
 *
 * 输出:
 *    TRUE--正在运行
 *    FALSE--停止运行
 */
static boolean LPLD_SDHC_IsRunning(void)
{
  return (0 != (SDHC->PRSSTAT & (SDHC_PRSSTAT_RTA_MASK | SDHC_PRSSTAT_WTA_MASK | SDHC_PRSSTAT_DLA_MASK | SDHC_PRSSTAT_CDIHB_MASK | SDHC_PRSSTAT_CIHB_MASK)));
}

/*
 * LPLD_SDHC_WaitStatus
 * 等待指定状态标志位置位
 * 
 * 参数:
 *    mask--状态标志位掩码
 *
 * 输出:
 *    状态标志
 */
static uint32 LPLD_SDHC_WaitStatus(uint32 mask)
{
  uint32 result;
  do
  {
    result = SDHC->IRQSTAT & mask;
  }
  while (0 == result);
  return result;
}

/*
 * LPLD_SDHC_Init
 * SDHC模块初始化函数
 * 
 * 参数:
 *    coreClk--系統主频，单位Hz
 *    baud--SDHC期望时钟频率，单位Hz
 *
 * 输出:
 *    SDHCSTA_OK--状态正常
 *    SDHCSTA_NOINIT--驱动未初始化
 *    SDHCSTA_NODISK--为插入卡
 *    SDHCSTA_PROTECT--卡写保护
 */
static SDHCRES LPLD_SDHC_Init(uint32 coreClk, uint32 baud)
{
  
  sdcard_ptr->CARD = ESDHC_CARD_NONE;
  
  //禁用GPIO的SDHC复用功能
  LPLD_SDHC_InitGPIO (0);
  
  //复位SDHC模块
  SDHC->SYSCTL = SDHC_SYSCTL_RSTA_MASK | SDHC_SYSCTL_SDCLKFS(0x80);
  while (SDHC->SYSCTL & SDHC_SYSCTL_RSTA_MASK)
  { };
  
  //初始化寄存器值
  SDHC->VENDOR = 0;
  SDHC->BLKATTR = SDHC_BLKATTR_BLKCNT(1) | SDHC_BLKATTR_BLKSIZE(512);
  SDHC->PROCTL = SDHC_PROCTL_EMODE(ESDHC_PROCTL_EMODE_LITTLE) | SDHC_PROCTL_D3CD_MASK;
  SDHC->WML = SDHC_WML_RDWML(2) | SDHC_WML_WRWML(1);
  
  //设置SDHC初始化时钟，最好不要超过400kHz
  LPLD_SDHC_SetBaudrate (coreClk, baud);
  
  //等待
  while (SDHC->PRSSTAT & (SDHC_PRSSTAT_CIHB_MASK | SDHC_PRSSTAT_CDIHB_MASK))
  { };
  
  //使能GPIO的SDHC复用
  LPLD_SDHC_InitGPIO (0xFFFF);
  
  //使能各种请求
  SDHC->IRQSTAT = 0xFFFF;
  SDHC->IRQSTATEN = SDHC_IRQSTATEN_DEBESEN_MASK | SDHC_IRQSTATEN_DCESEN_MASK | SDHC_IRQSTATEN_DTOESEN_MASK
    | SDHC_IRQSTATEN_CIESEN_MASK | SDHC_IRQSTATEN_CEBESEN_MASK | SDHC_IRQSTATEN_CCESEN_MASK | SDHC_IRQSTATEN_CTOESEN_MASK
      | SDHC_IRQSTATEN_BRRSEN_MASK | SDHC_IRQSTATEN_BWRSEN_MASK | SDHC_IRQSTATEN_CRMSEN_MASK
        | SDHC_IRQSTATEN_TCSEN_MASK | SDHC_IRQSTATEN_CCSEN_MASK;
  
  //等待80个初始时钟
  SDHC->SYSCTL |= SDHC_SYSCTL_INITA_MASK;
  while (SDHC->SYSCTL & SDHC_SYSCTL_INITA_MASK)
  { };
  
  //检查卡是否插入
  if (SDHC->PRSSTAT & SDHC_PRSSTAT_CINS_MASK)
  {
    sdcard_ptr->CARD = ESDHC_CARD_UNKNOWN;
  }
  else
  {
    sdcard_ptr->STATUS = SDHCSTA_NODISK;
  }
  SDHC->IRQSTAT |= SDHC_IRQSTAT_CRM_MASK;
  
  return SDHCRES_OK;
}

/*
 * LPLD_SDHC_SendCommand
 * 向SD卡发送指定CMD命令
 * 
 * 参数:
 *    command--SDHC命令信息结构体
 *
 * 输出:
 *    SDHCRES--磁盘功能返回值
 */
static SDHCRES LPLD_SDHC_SendCommand(ESDHC_COMMAND_STRUCT_PTR command)
{
  uint32 xfertyp;
  uint32 blkattr;
  
  //检查命令
  xfertyp = command->COMMAND;
  
  if (ESDHC_XFERTYP_CMDTYP_RESUME == ((xfertyp & SDHC_XFERTYP_CMDTYP_MASK) >> SDHC_XFERTYP_CMDTYP_SHIFT))
  {
    //恢复类型命令必须设置DPSEL位
    xfertyp |= SDHC_XFERTYP_DPSEL_MASK;
  }
  
  if ((0 != command->BLOCKS) && (0 != command->BLOCKSIZE))
  {
    xfertyp |= SDHC_XFERTYP_DPSEL_MASK;
    if (command->BLOCKS != 1)
    {
      //多块传输
      xfertyp |= SDHC_XFERTYP_MSBSEL_MASK;
    }
    if ((uint32)-1 == command->BLOCKS)
    {
      //大量传输
      blkattr = SDHC_BLKATTR_BLKSIZE(command->BLOCKSIZE) | SDHC_BLKATTR_BLKCNT(0xFFFF);
    }
    else
    {
      blkattr = SDHC_BLKATTR_BLKSIZE(command->BLOCKSIZE) | SDHC_BLKATTR_BLKCNT(command->BLOCKS);
      xfertyp |= SDHC_XFERTYP_BCEN_MASK;
    }
  }
  else
  {
    blkattr = 0;
  }
  
  //卡移除状态清除
  SDHC->IRQSTAT |= SDHC_IRQSTAT_CRM_MASK;
  
  //等待CMD线空闲
  while (SDHC->PRSSTAT & SDHC_PRSSTAT_CIHB_MASK)
  { };
  
  //初始化命令
  SDHC->CMDARG = command->ARGUMENT;
  SDHC->BLKATTR = blkattr;
  SDHC->DSADDR = 0;
  
  //发送命令
  SDHC->XFERTYP = xfertyp;
  
  //等待响应
  if (LPLD_SDHC_WaitStatus (SDHC_IRQSTAT_CIE_MASK | SDHC_IRQSTAT_CEBE_MASK | SDHC_IRQSTAT_CCE_MASK | SDHC_IRQSTAT_CC_MASK) != SDHC_IRQSTAT_CC_MASK)
  {
    SDHC->IRQSTAT |= SDHC_IRQSTAT_CTOE_MASK | SDHC_IRQSTAT_CIE_MASK | SDHC_IRQSTAT_CEBE_MASK | SDHC_IRQSTAT_CCE_MASK | SDHC_IRQSTAT_CC_MASK;
    return SDHCRES_ERROR;
  }
  
  //检查卡是否移除
  if (SDHC->IRQSTAT & SDHC_IRQSTAT_CRM_MASK)
  {
    SDHC->IRQSTAT |= SDHC_IRQSTAT_CTOE_MASK | SDHC_IRQSTAT_CC_MASK;
    sdcard_ptr->STATUS = SDHCSTA_NODISK;
    return SDHCRES_NOTRDY;
  }
  
  //获取响应
  if (SDHC->IRQSTAT & SDHC_IRQSTAT_CTOE_MASK)
  {
    SDHC->IRQSTAT |= SDHC_IRQSTAT_CTOE_MASK | SDHC_IRQSTAT_CC_MASK;
    return SDHCRES_NONRSPNS;
  }
  if ((xfertyp & SDHC_XFERTYP_RSPTYP_MASK) != SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO))
  {
    command->RESPONSE[0] = SDHC->CMDRSP[0];
    if ((xfertyp & SDHC_XFERTYP_RSPTYP_MASK) == SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_136))
    {
      command->RESPONSE[1] = SDHC->CMDRSP[1];
      command->RESPONSE[2] = SDHC->CMDRSP[2];
      command->RESPONSE[3] = SDHC->CMDRSP[3];
    }
  }
  SDHC->IRQSTAT |= SDHC_IRQSTAT_CC_MASK;
  
  return SDHCRES_OK;
}


/*
 * LPLD_SDHC_IOC
 * SDHC模块其他控制服务函数
 * 
 * 参数:
 *    cmd--SDHC模块控制命令
 *    *param_ptr--控制参数
 *
 * 输出:
 *    SDHCRES--磁盘功能返回值
 */
SDHCRES LPLD_SDHC_IOC(uint32 cmd, void *param_ptr)
{
  
  ESDHC_COMMAND_STRUCT    command;
  boolean                 mem, io, mmc, ceata, mp, hc;
  int32                  val;
  SDHCRES                 result = SDHCRES_OK;
  uint32 *             param32_ptr = param_ptr;
  
  switch (cmd)
  {
  case IO_IOCTL_ESDHC_INIT:  
    //初始化SDHC模块
    result = LPLD_SDHC_Init (g_core_clock*1000, 400000);
    if (SDHCRES_OK != result)
    {
      break;
    }
    
    mem = FALSE;
    io = FALSE;
    mmc = FALSE;
    ceata = FALSE;
    hc = FALSE;
    mp = FALSE;
    
    //CMD0 - 空闲命令，复位卡
    command.COMMAND = ESDHC_CMD0;
    command.ARGUMENT = 0;
    command.BLOCKS = 0;
    result = LPLD_SDHC_SendCommand (&command);
    if (result!=SDHCRES_OK)
    {
      sdcard_ptr->STATUS = SDHCSTA_NOINIT;
      break;
    }
    
    //CMD8 - 发送接口状态，检查是否支持高容量
    command.COMMAND = ESDHC_CMD8;
    command.ARGUMENT = 0x000001AA;
    command.BLOCKS = 0;
    result = LPLD_SDHC_SendCommand (&command);
    if (result==SDHCRES_ERROR)
    {
      sdcard_ptr->STATUS = SDHCSTA_NOINIT;
      break;
    }
    if (result == SDHCRES_OK)
    {
      if (command.RESPONSE[0] != command.ARGUMENT)
      {
        sdcard_ptr->STATUS = SDHCSTA_NOINIT;
        result = SDHCRES_ERROR;
        break;
      }
      hc = TRUE;
    }
    
    //CMD5 - 发送操作状态，测试IO
    command.COMMAND = ESDHC_CMD5;
    command.ARGUMENT = 0;
    command.BLOCKS = 0;      
    result = LPLD_SDHC_SendCommand (&command);
    if (result==SDHCRES_ERROR)
    {
      sdcard_ptr->STATUS = SDHCSTA_NOINIT;
      break;
    }
    if (result == SDHCRES_OK)
    {
      if (((command.RESPONSE[0] >> 28) & 0x07) && (command.RESPONSE[0] & 0x300000))
      {
        command.COMMAND = ESDHC_CMD5;
        command.ARGUMENT = 0x300000;
        command.BLOCKS = 0;
        val = 0;
        do
        {
          val++;
          if (result = LPLD_SDHC_SendCommand (&command))
          {
            break;
          }
        } while ((0 == (command.RESPONSE[0] & 0x80000000)) && (val < ESDHC_ALARM_FREQUENCY));
        if (SDHCRES_OK != result)
        {
          break;
        }
        if (command.RESPONSE[0] & 0x80000000)
        {
          io = TRUE;
        }
        if (command.RESPONSE[0] & 0x08000000)
        {
          mp = TRUE;
        }
      }
    }
    else
    {
      mp = TRUE;
    }
    
    if (mp)
    {
      //CMD55 - 特殊应用命令，检查MMC卡
      command.COMMAND = ESDHC_CMD55;
      command.ARGUMENT = 0;
      command.BLOCKS = 0;
      if ((result = LPLD_SDHC_SendCommand (&command))==SDHCRES_ERROR)
      {
        break;
      }
      if (result == SDHCRES_NONRSPNS)
      {
        //如果为MMC 或 CE-ATA 卡
        io = FALSE;
        mem = FALSE;
        hc = FALSE;
        
        //CMD1 - 发送测试命令，检查高容量支持
        command.COMMAND = ESDHC_CMD1;
        command.ARGUMENT = 0x40300000;
        command.BLOCKS = 0;
        if (result = LPLD_SDHC_SendCommand (&command))
        {
          break;
        }
        if (0x20000000 == (command.RESPONSE[0] & 0x60000000))
        {
          hc = TRUE;
        }
        mmc = TRUE;
        
        //CMD39 - 快速IO，检查CE-ATA的CE签名 */
        command.COMMAND = ESDHC_CMD39;
        command.ARGUMENT = 0x0C00;
        command.BLOCKS = 0;
        if (result = LPLD_SDHC_SendCommand (&command))
        {
          break;
        }
        if (0xCE == (command.RESPONSE[0] >> 8) & 0xFF)
        {
          //CMD39 - 快速IO，检查CE-ATA的AA签名 */
          command.COMMAND = ESDHC_CMD39;
          command.ARGUMENT = 0x0D00;
          command.BLOCKS = 0;
          if (result = LPLD_SDHC_SendCommand (&command))
          {
            break;
          }
          if (0xAA == (command.RESPONSE[0] >> 8) & 0xFF)
          {
            mmc = FALSE;
            ceata = TRUE;
          }
        }
      }
      else
      {
        //如果为SD卡
        //ACMD41 - 发送操作状态
        command.COMMAND = ESDHC_ACMD41;
        command.ARGUMENT = 0;
        command.BLOCKS = 0;
        if (result = LPLD_SDHC_SendCommand (&command))
        {
          sdcard_ptr->STATUS = SDHCSTA_NOINIT;
          break;
        }
        if (command.RESPONSE[0] & 0x300000)
        {
          val = 0;
          do
          {
            val++;
            
            //CMD55 + ACMD41 - 发送OCR
            command.COMMAND = ESDHC_CMD55;
            command.ARGUMENT = 0;
            command.BLOCKS = 0;
            if (result = LPLD_SDHC_SendCommand (&command))
            {
              break;
            }
            
            command.COMMAND = ESDHC_ACMD41;
            if (hc)
            {
              command.ARGUMENT = 0x40300000;
            }
            else
            {
              command.ARGUMENT = 0x00300000;
            }
            command.BLOCKS = 0;
            if (result = LPLD_SDHC_SendCommand (&command))
            {
              break;
            }
          } while ((0 == (command.RESPONSE[0] & 0x80000000)) && (val < ESDHC_ALARM_FREQUENCY));
          if (SDHCRES_OK != result)
          {
            break;
          }
          if (val >= ESDHC_ALARM_FREQUENCY)
          {
            hc = FALSE;
          }
          else
          {
            mem = TRUE;
            if (hc)
            {
              hc = FALSE;
              if (command.RESPONSE[0] & 0x40000000)
              {
                hc = TRUE;
              }
            }
          }
        }
      }
    }
    if (mmc)
    {
      sdcard_ptr->CARD = ESDHC_CARD_MMC;
    }
    if (ceata)
    {
      sdcard_ptr->CARD = ESDHC_CARD_CEATA;
    }
    if (io)
    {
      sdcard_ptr->CARD = ESDHC_CARD_SDIO;
    }
    if (mem)
    {
      sdcard_ptr->CARD = ESDHC_CARD_SD;
      if (hc)
      {
        sdcard_ptr->CARD = ESDHC_CARD_SDHC;
      }
    }
    if (io && mem)
    {
      sdcard_ptr->CARD = ESDHC_CARD_SDCOMBO;
      if (hc)
      {
        sdcard_ptr->CARD = ESDHC_CARD_SDHCCOMBO;
      }
    }
    
    //禁用GPIO的SDHC复用
    LPLD_SDHC_InitGPIO (0);
    
    //设置SDHC工作状态下的默认波特率
    LPLD_SDHC_SetBaudrate (g_core_clock*1000, 25000000);
    
    //使能GPIO的SDHC复用
    LPLD_SDHC_InitGPIO (0xFFFF);
    
    if(result == SDHCRES_OK)
    {
      sdcard_ptr->STATUS = SDHCSTA_OK;
    }
    break;
  case IO_IOCTL_ESDHC_SEND_COMMAND:
    result = LPLD_SDHC_SendCommand ((ESDHC_COMMAND_STRUCT_PTR)param32_ptr);
    break;
  case IO_IOCTL_ESDHC_GET_BAUDRATE:
    if (NULL == param32_ptr)
    {
      result = SDHCRES_ERROR;
    }
    else
    {
      //获取波特率
      val = ((SDHC->SYSCTL & SDHC_SYSCTL_SDCLKFS_MASK) >> SDHC_SYSCTL_SDCLKFS_SHIFT) << 1;
      val *= ((SDHC->SYSCTL & SDHC_SYSCTL_DVS_MASK) >> SDHC_SYSCTL_DVS_SHIFT) + 1;
      *param32_ptr = (uint32)((g_core_clock*1000 / val));
    }
    break;
  case IO_IOCTL_ESDHC_SET_BAUDRATE:
    if (NULL == param32_ptr)
    {
      result = SDHCRES_ERROR;
    }
    else if (0 == (*param32_ptr))
    {
      result = SDHCRES_ERROR;
    }
    else
    {
      if (! LPLD_SDHC_IsRunning ())
      {
        //禁用GPIO的SDHC复用
        LPLD_SDHC_InitGPIO (0);
        
        //设置波特率
        LPLD_SDHC_SetBaudrate (g_core_clock*1000, *param32_ptr);
        
        //使能GPIO的SDHC复用
        LPLD_SDHC_InitGPIO (0xFFFF);
      }
      else
      {
        result = SDHCRES_ERROR;
      }
    }
    break;
  case IO_IOCTL_ESDHC_GET_BUS_WIDTH:
    if (NULL == param32_ptr)
    {
      result = SDHCRES_ERROR;
    }
    else
    {
      //获得SDHC总线宽度
      val = (SDHC->PROCTL & SDHC_PROCTL_DTW_MASK) >> SDHC_PROCTL_DTW_SHIFT;
      if (ESDHC_PROCTL_DTW_1BIT == val)
      {
        *param32_ptr = ESDHC_BUS_WIDTH_1BIT;
      }
      else if (ESDHC_PROCTL_DTW_4BIT == val)
      {
        *param32_ptr = ESDHC_BUS_WIDTH_4BIT;
      }
      else if (ESDHC_PROCTL_DTW_8BIT == val)
      {
        *param32_ptr = ESDHC_BUS_WIDTH_8BIT;
      }
      else
      {
        result = SDHCRES_ERROR;
      }
    }
    break;
  case IO_IOCTL_ESDHC_SET_BUS_WIDTH:
    if (NULL == param32_ptr)
    {
      result = SDHCRES_ERROR;
    }
    else
    {
      //设置SDHC总线宽度
      if (! LPLD_SDHC_IsRunning ())
      {
        if (ESDHC_BUS_WIDTH_1BIT == *param32_ptr)
        {
          SDHC->PROCTL &= (~ SDHC_PROCTL_DTW_MASK);
          SDHC->PROCTL |= SDHC_PROCTL_DTW(ESDHC_PROCTL_DTW_1BIT);
        }
        else if (ESDHC_BUS_WIDTH_4BIT == *param32_ptr)
        {
          SDHC->PROCTL &= (~ SDHC_PROCTL_DTW_MASK);
          SDHC->PROCTL |= SDHC_PROCTL_DTW(ESDHC_PROCTL_DTW_4BIT);
        }
        else if (ESDHC_BUS_WIDTH_8BIT == *param32_ptr)
        {
          SDHC->PROCTL &= (~ SDHC_PROCTL_DTW_MASK);
          SDHC->PROCTL |= SDHC_PROCTL_DTW(ESDHC_PROCTL_DTW_8BIT);
        }
        else
        {
          result = SDHCRES_ERROR;
        }
      }
      else
      {
        result = SDHCRES_ERROR;
      }
    }
    break;
  case IO_IOCTL_ESDHC_GET_CARD:
    if (NULL == param32_ptr)
    {
      result = SDHCRES_ERROR;
    }
    else
    {
      //等待80个时钟
      SDHC->SYSCTL |= SDHC_SYSCTL_INITA_MASK;
      while (SDHC->SYSCTL & SDHC_SYSCTL_INITA_MASK)
      { };
      
      //更新并返回卡实际状态
      if (SDHC->IRQSTAT & SDHC_IRQSTAT_CRM_MASK)
      {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_CRM_MASK;
        sdcard_ptr->CARD = ESDHC_CARD_NONE;
        sdcard_ptr->STATUS = SDHCSTA_NODISK;
      }
      if (SDHC->PRSSTAT & SDHC_PRSSTAT_CINS_MASK)
      {
        if (ESDHC_CARD_NONE == sdcard_ptr->CARD)
        {
          sdcard_ptr->CARD = ESDHC_CARD_UNKNOWN;
        }
      }
      else
      {
        sdcard_ptr->CARD = ESDHC_CARD_NONE;
      }
      *param32_ptr = sdcard_ptr->CARD;
    }
    break;
    
  case IO_IOCTL_FLUSH_OUTPUT:
    //等待传输完成
    LPLD_SDHC_WaitStatus (SDHC_IRQSTAT_TC_MASK);
    if (SDHC->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
    {
      SDHC->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK;
      result = SDHCRES_ERROR;
    }
    SDHC->IRQSTAT |= SDHC_IRQSTAT_TC_MASK | SDHC_IRQSTAT_BRR_MASK | SDHC_IRQSTAT_BWR_MASK;
    break;
  default:
    result = SDHCRES_ERROR;
    break;
  }
  
  
  return result;
}

/*
 * LPLD_SDHC_Read
 * SDHC读操作
 * 
 * 参数:
 *    *data_ptr--存储数据地址指针
 *    n--待读的数据长度
 *
 * 输出:
 *    SDHCRES--磁盘功能返回值
 */
static SDHCSTATUS LPLD_SDHC_Read(uint8 *data_ptr, int32 n)
{
  uint32 buffer;
  int32 remains;
  
  remains = n;
  if (((uint32)data_ptr & 0x03) == 0)
  {    
    //数据位字对齐，可以以最快的速度直接从寄存器拷贝
    while (remains >= 4)
    {
      if (SDHC->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
      {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BRR_MASK;
        return SDHCRES_ERROR;
      }
      
      //等待，直到收到的数据达到水印长度或传输完成
      while ((0 == (SDHC->PRSSTAT & SDHC_PRSSTAT_BREN_MASK)) && (SDHC->PRSSTAT & SDHC_PRSSTAT_DLA_MASK))
      { };
      
      *((uint32 *)data_ptr) = SDHC->DATPORT;
      data_ptr += 4;
      remains -= 4;
    }
  }
  else
  {
    //非对齐数据，读到临时区域并以字节复制
    while (remains >= 4)
    {
      if (SDHC->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
      {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BRR_MASK;
        return SDHCRES_ERROR;
      }
      
      //等待，直到收到的数据达到水印长度或传输完成
      while ((0 == (SDHC->PRSSTAT & SDHC_PRSSTAT_BREN_MASK)) && (SDHC->PRSSTAT & SDHC_PRSSTAT_DLA_MASK))
      { };
      
      buffer = SDHC->DATPORT;
      
      *data_ptr++ = buffer & 0xFF;
      *data_ptr++ = (buffer >> 8) & 0xFF;
      *data_ptr++ = (buffer >> 16) & 0xFF;
      *data_ptr++ = (buffer >> 24) & 0xFF;
      
      remains -= 4;
    }      
  }
  
  if (remains)
  {
    //剩下的少于单字长度数据
    if (SDHC->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
    {
      SDHC->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BRR_MASK;
      return SDHCRES_ERROR;
    }
    
    //等待，直到收到的数据达到水印长度或传输完成
    while ((0 == (SDHC->PRSSTAT & SDHC_PRSSTAT_BREN_MASK)) && (SDHC->PRSSTAT & SDHC_PRSSTAT_DLA_MASK))
    { };
    
    buffer = SDHC->DATPORT;
    while (remains)
    {
      
      *data_ptr++ = buffer & 0xFF;
      buffer >>= 8;
      
      remains--;
    }
  }
  
  if (SDHC->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
  {
    SDHC->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BRR_MASK;
    return SDHCRES_ERROR;
  }
  
  return (n - remains);
}

/*
 * LPLD_SDHC_Write
 * SDHC写操作
 * 
 * 参数:
 *    *data_ptr--存储数据地址指针
 *    n--待写的数据长度
 *
 * 输出:
 *    SDHCRES--磁盘功能返回值
 */
static SDHCSTATUS LPLD_SDHC_Write(uint8 *data_ptr, int32 n)
{
  uint8 *udata_ptr;
  uint32 buffer;
  int32 remains;
  
  //复制数据指针
  udata_ptr = (uint8 *)data_ptr;
  
  remains = n;
  if (((uint32)udata_ptr & 0x03) == 0)
  {
    //数据位字对齐，可以以最快的速度直接拷贝到寄存器
    while (remains >= 4)
    {
      if (SDHC->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
      {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BWR_MASK;
        return SDHCRES_ERROR;
      }
      
      //等待，直到水印空间可用 
      while (0 == (SDHC->PRSSTAT & SDHC_PRSSTAT_BWEN_MASK))
      { };
      
      SDHC->DATPORT = *((uint32 *)udata_ptr);
      udata_ptr += 4;
      remains -= 4;
    }
  }
  else
  {
    //非对齐数据，写到临时区域并以字节复制
    while (remains >= 4)
    {
      if (SDHC->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
      {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BWR_MASK;
        return SDHCRES_ERROR;
      }
      
      //等待，直到水印空间可用 
      while (0 == (SDHC->PRSSTAT & SDHC_PRSSTAT_BWEN_MASK))
      { };
      
      buffer  = (*udata_ptr++);
      buffer |= (*udata_ptr++) << 8;
      buffer |= (*udata_ptr++) << 16;
      buffer |= (*udata_ptr++) << 24;
      
      //等待，直到水印空间可用 
      while (0 == (SDHC->PRSSTAT & SDHC_PRSSTAT_BWEN_MASK))
      { };
      
      SDHC->DATPORT = buffer;
      remains -= 4;
    }      
  }
  
  if (remains)
  {
    //剩余少于单字长度的数据
    if (SDHC->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
    {
      SDHC->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BWR_MASK;
      return SDHCRES_ERROR;
    }
    
    buffer = 0xFFFFFFFF;
    while (remains)
    {
      buffer <<= 8;
      buffer |= udata_ptr[remains];
      remains--;
    }
    
    //等待，直到水印空间可用 
    while (0 == (SDHC->PRSSTAT & SDHC_PRSSTAT_BWEN_MASK))
    { };
    
    SDHC->DATPORT = buffer;        
  }
  
  if (SDHC->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
  {
    SDHC->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BWR_MASK;
    return SDHCRES_ERROR;
  }
  
  return (n - remains);
}

/*
 * LPLD_SDHC_InitCard
 * 初始化SDHC模块及SD卡，设置正常工作波特率为40MHz
 * 
 * 参数:
 *    无
 *
 * 输出:
 *    SDHCSTA_OK--状态正常
 *    SDHCSTA_NOINIT--驱动未初始化
 *    SDHCSTA_NODISK--为插入卡
 *    SDHCSTA_PROTECT--卡写保护
 */
SDHCSTATUS LPLD_SDHC_InitCard(void)
{
  uint32 param, c_size, c_size_mult, read_bl_len, time_out = 0;
  ESDHC_COMMAND_STRUCT command;
  SDHCSTATUS result;
  
  //分配SD卡信息结构体的数据空间并初始化
  sdcard_ptr = (SDCARD_STRUCT_PTR)malloc(sizeof(SDCARD_STRUCT));
  sdcard_ptr->CARD = ESDHC_CARD_NONE;
  sdcard_ptr->TIMEOUT = 0;
  sdcard_ptr->NUM_BLOCKS = 0;
  sdcard_ptr->ADDRESS = 0;
  sdcard_ptr->HC = FALSE;
  sdcard_ptr->VERSION2 = FALSE;
  sdcard_ptr->STATUS = SDHCSTA_OK;
   
  while(time_out < 1000)
  {
    
    //初始化SDHC模块并检测卡
    if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_INIT, NULL)))
    {
      continue;
    }
    
    //SDHC检查
    param = 0;
    if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_GET_CARD, &param)))
    {
      continue;
    }
    if ((ESDHC_CARD_SD == param) || (ESDHC_CARD_SDHC == param) || (ESDHC_CARD_SDCOMBO == param) || (ESDHC_CARD_SDHCCOMBO == param))
    {
      if ((ESDHC_CARD_SDHC == param) || (ESDHC_CARD_SDHCCOMBO == param))
      {
        sdcard_ptr->HC = TRUE;
        break;
      }
      else if(ESDHC_CARD_SD == param)
      {
        break;
      }
    }
    else
    {
      continue;
    }
    time_out++;
  }
  
  if(time_out >= 1000)
    return SDHCRES_NOTRDY;
  
  //卡识别
  command.COMMAND = ESDHC_CMD2;
  command.ARGUMENT = 0;
  command.BLOCKS = 0;
  if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_SEND_COMMAND, &command)))
  {
    return result;
  }
  
  //获取卡地址
  command.COMMAND = ESDHC_CMD3;
  command.ARGUMENT = 0;
  command.BLOCKS = 0;
  if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_SEND_COMMAND, &command)))
  {
    return result;
  }
  sdcard_ptr->ADDRESS = command.RESPONSE[0] & 0xFFFF0000;
  
  //获取卡参数
  command.COMMAND = ESDHC_CMD9;
  command.ARGUMENT = sdcard_ptr->ADDRESS;
  command.BLOCKS = 0;
  if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_SEND_COMMAND, &command)))
  {
    return result;
  }
  if (0 == (command.RESPONSE[3] & 0x00C00000))
  {
    read_bl_len = (command.RESPONSE[2] >> 8) & 0x0F;
    c_size = command.RESPONSE[2] & 0x03;
    c_size = (c_size << 10) | (command.RESPONSE[1] >> 22);
    c_size_mult = (command.RESPONSE[1] >> 7) & 0x07;
    sdcard_ptr->NUM_BLOCKS = (c_size + 1) * (1 << (c_size_mult + 2)) * (1 << (read_bl_len - 9));
  }
  else
  {
    sdcard_ptr->VERSION2 = TRUE;
    c_size = (command.RESPONSE[1] >> 8) & 0x003FFFFF;
    sdcard_ptr->NUM_BLOCKS = (c_size + 1) << 10;
  }
  
  //设置正常工作波特率为40MHz
  //param = 40000000;    
  param = 25000000;      
  if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_SET_BAUDRATE, &param)))
  {
    return result;
  }
  
  //选择卡
  command.COMMAND = ESDHC_CMD7;
  command.ARGUMENT = sdcard_ptr->ADDRESS;
  command.BLOCKS = 0;
  if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_SEND_COMMAND, &command)))
  {
    return result;
  }
  
  //设置块大小为512字节
  command.COMMAND = ESDHC_CMD16;
  command.ARGUMENT = IO_SDCARD_BLOCK_SIZE;
  command.BLOCKS = 0;
  if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_SEND_COMMAND, &command)))
  {
    return result;
  }
  
  if (ESDHC_BUS_WIDTH_4BIT == ESDHC_BUS_WIDTH_4BIT)
  {
    //特殊应用命令
    command.COMMAND = ESDHC_CMD55;
    command.ARGUMENT = sdcard_ptr->ADDRESS;
    command.BLOCKS = 0;
    if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_SEND_COMMAND, &command)))
    {
      return result;
    }
    
    //设置总线宽度为4bit
    command.COMMAND = ESDHC_ACMD6;
    command.ARGUMENT = 2;
    command.BLOCKS = 0;
    if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_SEND_COMMAND, &command)))
    {
      return result;
    }
    
    param = ESDHC_BUS_WIDTH_4BIT;
    if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_SET_BUS_WIDTH, &param)))
    {
      return result;
    }
  }
  
  return SDHCRES_OK;
}


/*
 * LPLD_SDHC_ReadBlocks
 * 读指定扇区指定长度的块
 * 
 * 参数:
 *    buff--存储读出数据的地址指针
 *    sector--开始的扇区号
 *    count--读出的扇区数（块数）
 *
 * 输出:
 *    SDHCRES--磁盘功能返回值
 */
SDHCRES LPLD_SDHC_ReadBlocks(uint8 *buff, uint32 sector, uint32 count)
{
  ESDHC_COMMAND_STRUCT command;
  int cnt;
  int32 result;
  
  //SD卡数据地址调节
  if (! sdcard_ptr->HC)
  {
    sector <<= IO_SDCARD_BLOCK_SIZE_POWER;
  }
  
  //设置读块命令
  if (count > 1)
  {
    command.COMMAND = ESDHC_CMD18;
  }   
  else
  {
    command.COMMAND = ESDHC_CMD17;
  }
  
  command.ARGUMENT = sector;
  command.BLOCKS = count;
  command.BLOCKSIZE = IO_SDCARD_BLOCK_SIZE;
  if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_SEND_COMMAND, &command)))
  {
    return (SDHCRES)result;
  }
  
  //读取数据
  for (cnt = 0; cnt < count; cnt++)
  {
    if (IO_SDCARD_BLOCK_SIZE != (result=LPLD_SDHC_Read(buff,IO_SDCARD_BLOCK_SIZE)))
    {
      return (SDHCRES)result;
    }
    buff += IO_SDCARD_BLOCK_SIZE;
  }
  
  //等待传输结束
  if (SDHCRES_OK !=(result=LPLD_SDHC_IOC (IO_IOCTL_FLUSH_OUTPUT, NULL)))
  {
    return (SDHCRES)result;
  }
  
  return (SDHCRES)result;
}


/*
 * LPLD_SDHC_WriteBlocks
 * 在指定扇区写入指定长度块数数据
 * 
 * 参数:
 *    buff--存储待写入数据的地址指针
 *    sector--开始的扇区号
 *    count--写入的扇区数（块数）
 *
 * 输出:
 *    SDHCRES--磁盘功能返回值
 */
SDHCRES LPLD_SDHC_WriteBlocks(uint8* buff, uint32 sector, uint32 count)
{
    ESDHC_COMMAND_STRUCT command;
    uint8               tmp[4];
    int32             cnt;
      int32 result;
    
 
    //SD卡数据地址调节
    if (! sdcard_ptr->HC)
    {
        sector <<= IO_SDCARD_BLOCK_SIZE_POWER;
    }

    //设置写块命令
    if (count > 1)
    {
        command.COMMAND = ESDHC_CMD25;
    }
    else
    {
        command.COMMAND = ESDHC_CMD24;
    }

    command.ARGUMENT = sector;
    command.BLOCKS = count;
    command.BLOCKSIZE = IO_SDCARD_BLOCK_SIZE;
    if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_SEND_COMMAND, &command)))
    {
        return (SDHCRES)result;
    }
    
    //写数据
    for (cnt = 0; cnt < count; cnt++)
    {
        if (IO_SDCARD_BLOCK_SIZE != (result=LPLD_SDHC_Write (buff, IO_SDCARD_BLOCK_SIZE)))
        {
            return (SDHCRES)result;
        }
        buff += IO_SDCARD_BLOCK_SIZE;
    }

    //等待传输结束
    if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_FLUSH_OUTPUT, NULL)))
    {
        return (SDHCRES)result;
    }

    //等待卡准备好/传输状态
    do
    {
        command.COMMAND = ESDHC_CMD13;
        command.ARGUMENT = sdcard_ptr->ADDRESS;
        command.BLOCKS = 0;
        if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_SEND_COMMAND, &command)))
        {
            return (SDHCRES)result;
        }

        //卡状态错误检查
        if (command.RESPONSE[0] & 0xFFD98008)
        {
            count = 0; /* necessary to get real number of written blocks */
            break;
        }

    } while (0x000000900 != (command.RESPONSE[0] & 0x00001F00));

    if (cnt != count)
    {
        //特殊应用命令
        command.COMMAND = ESDHC_CMD55;
        command.ARGUMENT = sdcard_ptr->ADDRESS;
        command.BLOCKS = 0;
        if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_SEND_COMMAND, &command)))
        {
            return (SDHCRES)result;
        }
                
        //使用ACMD22命令获得写入的块数量
        command.COMMAND = ESDHC_ACMD22;
        command.ARGUMENT = 0;
        command.BLOCKS = 1;
        command.BLOCKSIZE = 4;
        if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_ESDHC_SEND_COMMAND, &command)))
        {
            return (SDHCRES)result;
        }
        
        if (4 != (result=LPLD_SDHC_Read (tmp, 4)))
        {
            return (SDHCRES)result;
            
        }

        if (SDHCRES_OK != (result=LPLD_SDHC_IOC (IO_IOCTL_FLUSH_OUTPUT, NULL)))
        {
            return (SDHCRES)result;
        }

        count = (tmp[0] << 24) | (tmp[1] << 16) | (tmp[2] << 8) | tmp[3];
        if ((cnt < 0) || (cnt > count))
            return SDHCRES_ERROR;
    }
    
    return SDHCRES_OK;
}
