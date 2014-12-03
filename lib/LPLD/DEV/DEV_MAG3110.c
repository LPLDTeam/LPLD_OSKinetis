/**
 * @file DEV_MAG3110.c
 * @version 0.2[By LPLD]
 * @date 2013-10-3
 * @brief MAG3110三轴地磁传感器设备驱动程序
 *
 * 更改建议:可根据实际硬件修改
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
#include "DEV_MAG3110.h"

static void MAG3110_Delay(void);

/*
 *   LPLD_MAG3110_Init
 *   初始化MAG3110，包括初始化3110所需的I2C接口以及3110的寄存器
 *
 *   参数：
 *    无
 */
uint8 LPLD_MAG3110_Init(void)
{
  uint8 device_id;
  I2C_InitTypeDef i2c_init_param;
    
  //初始化MAG3110_I2CX
  i2c_init_param.I2C_I2Cx = MAG3110_I2CX;       //在DEV_MAG3110.h中修改该值
  i2c_init_param.I2C_ICR = MAG3110_SCL_200KHZ;  //可根据实际电路更改SCL频率
  i2c_init_param.I2C_SclPin = MAG3110_SCLPIN;   //在DEV_MAG3110.h中修改该值
  i2c_init_param.I2C_SdaPin = MAG3110_SDAPIN;   //在DEV_MAG3110.h中修改该值
  i2c_init_param.I2C_IntEnable = FALSE;
  i2c_init_param.I2C_Isr = NULL;
  i2c_init_param.I2C_OpenDrainEnable = FALSE;
  
  LPLD_I2C_Init(i2c_init_param);
  
  //读取设备ID
  device_id = LPLD_MAG3110_ReadReg(MAG3110_WHO_AM_I);
      
  //配置CTRL_REG2
  //自动复位模式
  LPLD_MAG3110_WriteReg(MAG3110_CTRL_REG2, CTRL_REG2_AUTO_MRST_EN_MARK); 
  //配置CTRL_REG1 
  LPLD_MAG3110_WriteReg(MAG3110_CTRL_REG1, CTRL_REG1_OS(0)|CTRL_REG1_AC_MARK);
  
  return device_id;
}

/*
 *   LPLD_MAG3110_WriteReg
 *   该函数用于配置MAG3110的寄存器
 *
 *   参数：
 *   RegisterAddress 
 *    |__ MAG3110寄存器地址
 *   Data
 *    |__ 具体写入的字节型数据 
 */
void LPLD_MAG3110_WriteReg(uint8 RegisterAddress, uint8 Data)
{
  //发送从机地址
  LPLD_I2C_StartTrans(MAG3110_I2CX, MAG3110_DEV_ADDR, I2C_MWSR);
  LPLD_I2C_WaitAck(MAG3110_I2CX, I2C_ACK_ON);
  
  //写MAG3110寄存器地址
  LPLD_I2C_WriteByte(MAG3110_I2CX, RegisterAddress);
  LPLD_I2C_WaitAck(MAG3110_I2CX, I2C_ACK_ON);
  
  //向寄存器中写具体数据
  LPLD_I2C_WriteByte(MAG3110_I2CX, Data);
  LPLD_I2C_WaitAck(MAG3110_I2CX, I2C_ACK_ON);

  LPLD_I2C_Stop(MAG3110_I2CX);

  //MAG3110_Delay();
}

/*
 *   LPLD_MAG3110_WriteReg
 *   该函数用于读取MAG3110的数据
 *
 *   参数：
 *     RegisterAddress 
 *        |__ MAG3110寄存器地址
 *   返回值
 *      加速传感器的测量值
 */
uint8 LPLD_MAG3110_ReadReg(uint8 RegisterAddress)
{
  uint8 result;

  //发送从机地址
  LPLD_I2C_StartTrans(MAG3110_I2CX, MAG3110_DEV_ADDR, I2C_MWSR);
  LPLD_I2C_WaitAck(MAG3110_I2CX, I2C_ACK_ON);

  //写MAG3110寄存器地址
  LPLD_I2C_WriteByte(MAG3110_I2CX, RegisterAddress);
  LPLD_I2C_WaitAck(MAG3110_I2CX, I2C_ACK_ON);

  //再次产生开始信号
  LPLD_I2C_ReStart(MAG3110_I2CX);

  //发送从机地址和读取位
  LPLD_I2C_WriteByte(MAG3110_I2CX, MAG3110_DEV_READ);
  LPLD_I2C_WaitAck(MAG3110_I2CX, I2C_ACK_ON);

  //转换主机模式为读
  LPLD_I2C_SetMasterWR(MAG3110_I2CX, I2C_MRSW);

  //关闭应答ACK
  LPLD_I2C_WaitAck(MAG3110_I2CX, I2C_ACK_OFF);//关闭ACK

  //读IIC数据
  result =LPLD_I2C_ReadByte(MAG3110_I2CX);
  LPLD_I2C_WaitAck(MAG3110_I2CX, I2C_ACK_ON);

  //发送停止信号
  LPLD_I2C_Stop(MAG3110_I2CX);

   //读IIC数据
  result = LPLD_I2C_ReadByte(MAG3110_I2CX);

  MAG3110_Delay();
  
  return result;
}

/*
 * 函数功能：读MAA7660加速度输出
 * 参数
 *       Status - 数据寄存器状态
 *       Regs_Addr - 数据寄存器地址
 * 函数返回值：加速度值（int8）
 */   
int16 LPLD_MAG3110_GetResult(uint8 Status, uint8 Regs_Addr) 
{
  uint8 ret=0;
  uint16 cnt=0;
  int16 result,temp;
  
  if(Regs_Addr>MAG3110_OUT_Z_LSB)
    return 0;
  
  // 等待转换完成并取出值 
  while(!(ret&Status)) 
  {
    ret = LPLD_MAG3110_ReadReg( MAG3110_DR_STATUS);
    if(++cnt>500)
      break;
  }
  
  result= LPLD_MAG3110_ReadReg( Regs_Addr);
  temp  = LPLD_MAG3110_ReadReg( Regs_Addr+1);
  result=result<<8;
  result=result|temp;
  
  return result;
}


/*
 * 延时函数
 */
static void MAG3110_Delay(void){
  int n;
  for(n=1;n<200;n++) {
    asm("nop");
  }
}