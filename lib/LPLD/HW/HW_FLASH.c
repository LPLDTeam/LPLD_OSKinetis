/**
 * @file HW_FLASH.c
 * @version 3.0[By LPLD]
 * @date 2013-06-18
 * @brief FTFL底层模块相关函数
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
 */
#include "common.h"
#include "HW_FLASH.h"

/*
 * LPLD_Flash_Init
 * Flash模块初始化
 * 
 * 参数:
 *    无
 *
 * 输出:
 *    无
 */
void LPLD_Flash_Init(void)
{
  //检查Flash访问错误
  if (FTFL->FSTAT & FTFL_FSTAT_ACCERR_MASK)
  {
    FTFL->FSTAT |= FTFL_FSTAT_ACCERR_MASK;       //清除错误标志
  }
  //检查保护错误
  else if (FTFL->FSTAT & FTFL_FSTAT_FPVIOL_MASK)
  {
    FTFL->FSTAT |= FTFL_FSTAT_FPVIOL_MASK;
  }
  //检查读冲突错误
  else if (FTFL->FSTAT & FTFL_FSTAT_RDCOLERR_MASK)
  {
    FTFL->FSTAT |= FTFL_FSTAT_RDCOLERR_MASK;
  }
  
  //禁用Flash模块的数据缓存
  FMC->PFB0CR &= ~FMC_PFB0CR_B0DCE_MASK;
  FMC->PFB1CR &= ~FMC_PFB1CR_B1DCE_MASK;
} 


/*
 * LPLD_Flash_SectorErase
 * 擦除Flash扇区
 * 
 * 参数:
 *    FlashPtr--扇区地址指针，即扇区号乘以2048
 *
 * 输出:
 *    错误代码
 */
uint8 LPLD_Flash_SectorErase(uint32 FlashPtr)
{
  uint8 Return = FLASH_OK;
  
  //等待CCIF置1
  while (!(FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK)){};
  //写入命令道FCCOB寄存器
  FTFL->FCCOB0 = FLASH_CMD_ERSSCR;
  FTFL->FCCOB1 = (uint8) (FlashPtr >> 16);
  FTFL->FCCOB2 = (uint8)((FlashPtr >> 8 ) & 0xFF);
  FTFL->FCCOB3 = (uint8)( FlashPtr & 0xFF);
  
  //执行命令
  FTFL->FSTAT |= FTFL_FSTAT_CCIF_MASK;    
  //等待命令完成
  while (!(FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK)) {};
  
  //检查Flash访问错误
  if (FTFL->FSTAT & FTFL_FSTAT_ACCERR_MASK)
  {
    FTFL->FSTAT |= FTFL_FSTAT_ACCERR_MASK;       //清除错误标志
    Return |= FLASH_FACCERR;                    //更新返回值
  }
  //检查Flash保护标志
  else if (FTFL->FSTAT & FTFL_FSTAT_FPVIOL_MASK)
  {
    FTFL->FSTAT |= FTFL_FSTAT_FPVIOL_MASK;
    Return |= FLASH_FPVIOL;
  }
  //检查Flash读冲突错误
  else if (FTFL->FSTAT & FTFL_FSTAT_RDCOLERR_MASK)
  {
    FTFL->FSTAT |= FTFL_FSTAT_RDCOLERR_MASK;
    Return |= FLASH_RDCOLERR;
  }
  //检查MGSTAT0非可纠正错误
  else if (FTFL->FSTAT & FTFL_FSTAT_MGSTAT0_MASK)
  {
    Return |= FLASH_MGSTAT0;
  } 
  
  return  Return;
}

/*
 * LPLD_Flash_ByteProgram
 * 编程Flash
 * 
 * 参数:
 *    FlashStartAdd--Flash编程起始地址
 *    *DataSrcPtr--数据源指针
 *    NumberOfBytes--数据字节长度
 *
 * 输出:
 *    错误代码
 */
uint8 LPLD_Flash_ByteProgram(uint32 FlashStartAdd, uint32 *DataSrcPtr, uint32 NumberOfBytes)
{
  uint8 Return = FLASH_OK;
  
  uint32 size_buffer;
  
  if (NumberOfBytes == 0)
  {
    return FLASH_CONTENTERR;
  }
  else
  {
    size_buffer = (NumberOfBytes - 1)/4 + 1;
  }
  //等待直到CCIF设置
  while (!(FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK)){};

  while ((size_buffer) && (Return == FLASH_OK))
  {
    //写FLASH命令到FCCOB
    FTFL->FCCOB0 = FLASH_CMD_PGM4;
    FTFL->FCCOB1 = (uint8)(FlashStartAdd >> 16);
    FTFL->FCCOB2 = (uint8)((FlashStartAdd >> 8) & 0xFF);
    FTFL->FCCOB3 = (uint8)(FlashStartAdd & 0xFF);
    //如果K60设置成为小端格式，进行如下操作
    FTFL->FCCOB4 = (uint8)(*((uint8*)DataSrcPtr+3));
    FTFL->FCCOB5 = (uint8)(*((uint8*)DataSrcPtr+2));
    FTFL->FCCOB6 = (uint8)(*((uint8*)DataSrcPtr+1));
    FTFL->FCCOB7 = (uint8)(*((uint8*)DataSrcPtr+0));
    //如果K60设置成为大端格式，进行如下操作
    //FTFL_FCCOB4 = (uint8)(*((uint_8*)DataSrcPtr+0));
    //FTFL_FCCOB5 = (uint8)(*((uint_8*)DataSrcPtr+1));
    //FTFL_FCCOB6 = (uint8)(*((uint_8*)DataSrcPtr+2));
    //FTFL_FCCOB7 = (uint8)(*((uint_8*)DataSrcPtr+3));
    //设置FLASH加载命令
    FTFL->FSTAT |= FTFL_FSTAT_CCIF_MASK;    
    //等待命令完成
    while (!(FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK)) {};
    
    //检测FLASH访问错误
    if (FTFL->FSTAT & FTFL_FSTAT_ACCERR_MASK)
    {
        
        FTFL->FSTAT |= FTFL_FSTAT_ACCERR_MASK;
    
        Return |= FLASH_FACCERR;
    }
    //检测FLASH保护错误
    else if (FTFL->FSTAT & FTFL_FSTAT_FPVIOL_MASK)
    {
        FTFL->FSTAT |= FTFL_FSTAT_FPVIOL_MASK;
    
        Return |= FLASH_FPVIOL;
    }
    
    else if (FTFL->FSTAT & FTFL_FSTAT_RDCOLERR_MASK)
    {
        FTFL->FSTAT |= FTFL_FSTAT_RDCOLERR_MASK;
    
        Return |= FLASH_RDCOLERR;
    }
    else if (FTFL->FSTAT & FTFL_FSTAT_MGSTAT0_MASK)
    {
        Return |= FLASH_MGSTAT0;
    } 
    size_buffer --;
    (uint32*)DataSrcPtr++;
    FlashStartAdd +=4;
  } 
  return  Return;
} 
