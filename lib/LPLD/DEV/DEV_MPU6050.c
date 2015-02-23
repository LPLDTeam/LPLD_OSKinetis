/**
 * @file DEV_MPU6050.c
 * @version 0.1[By LPLD]
 * @date 2015-02-22
 * @brief MPU60506轴运动处理组件驱动程序
 *
 * 更改建议:可根据实际硬件修改
 *
 * 十分感谢该文件作者对底层库的贡献！
 * http://www.lpld.cn
 * mail:support@lpld.cn
 * 
 * 文件出处：LPLD_K60底层库群
 * 作者：投笔从戎
 */
#include "common.h"
#include "DEV_MPU6050.h"

I2C_InitTypeDef MPU6050_init_struct;

/*
 *   MPU6050_WriteReg
 *   MPU6050写寄存器状态
 *
 *   参数：
 *    RegisterAddress 寄存器地址
 *    Data 所需要写得内容
*
 *   返回值
 *    无
 */
void MPU6050_WriteReg(uint8 RegisterAddress, uint8 Data)
{
  //发送从机地址
  LPLD_I2C_StartTrans(I2C1, SlaveAddress, I2C_MWSR);
  LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);
  
  //写MPU6050寄存器地址
  LPLD_I2C_WriteByte(I2C1, RegisterAddress);
  LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);
  
  //向寄存器中写具体数据
  LPLD_I2C_WriteByte(I2C1, Data);
  LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

  LPLD_I2C_Stop(I2C1);
}

/*
 *   MPU6050_ReadReg
 *   MPU6050读寄存器状态
 *
 *   参数：
 *    RegisterAddress 寄存器地址
 *
 *   返回值
 *    所读寄存器状态
 */
uint8 MPU6050_ReadReg(uint8 RegisterAddress)
{
  uint8 result;

  //发送从机地址
  LPLD_I2C_StartTrans(I2C1, SlaveAddress, I2C_MWSR);
  LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

  //写MPU6050寄存器地址
  LPLD_I2C_WriteByte(I2C1, RegisterAddress);
  LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);
  
  //再次产生开始信号
  LPLD_I2C_ReStart(I2C1);

  //发送从机地址和读取位
  LPLD_I2C_WriteByte(I2C1, (SlaveAddress<<1)|I2C_MRSW);
  LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

  //转换主机模式为读
  LPLD_I2C_SetMasterWR(I2C1, I2C_MRSW);

  //关闭应答ACK
  LPLD_I2C_WaitAck(I2C1, I2C_ACK_OFF);//关闭ACK

  //读IIC数据
  result =LPLD_I2C_ReadByte(I2C1);
  LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

  //发送停止信号
  LPLD_I2C_Stop(I2C1);

  //读IIC数据
  result = LPLD_I2C_ReadByte(I2C1);
  
  LPLD_SYSTICK_DelayMs(1);

  return result;
}

/*
 *   MPU6050_GetResult
 *   获得MPU6050结果
 *
 *   参数：
 *    无
 *
 *   返回值
 *    转换结果 
 */
int16 MPU6050_GetResult(uint8 Regs_Addr)
{
  int16 result,temp;
  result = MPU6050_ReadReg(Regs_Addr);
  temp   = MPU6050_ReadReg(Regs_Addr+1);
  result=result<<8;
  result=result|temp;
  return result;
}

/*
 *   MPU6050_Init
 *   初始化MPU6050，包括初始化MPU6050所需的I2C接口以及MPU6050的寄存器
 *
 *   参数：
 *    无
 *
 *   返回值
 *    无
 */
void MPU6050_Init()
{
  //初始化MPU6050
  MPU6050_init_struct.I2C_I2Cx = MPU6050_I2CX;          //在MPU6050.h中修改该值
  MPU6050_init_struct.I2C_IntEnable = FALSE;
  MPU6050_init_struct.I2C_ICR = MPU6050_SCL_400KHZ;     //可根据实际电路更改SCL频率
  MPU6050_init_struct.I2C_SclPin = MPU6050_SCLPIN;      //在MPU6050.h中修改该值
  MPU6050_init_struct.I2C_SdaPin = MPU6050_SCLPIN;      //在MPU6050.h中修改该值
  MPU6050_init_struct.I2C_Isr = NULL;
  LPLD_I2C_Init(MPU6050_init_struct);
  
  LPLD_SYSTICK_DelayMs(1);
  
  MPU6050_WriteReg(PWR_MGMT_1,0x00);    //解除休眠状态
  MPU6050_WriteReg(SMPLRT_DIV,0x07);    //陀螺仪采样率，典型值：0x07(125Hz)
  MPU6050_WriteReg(CONFIG,0x06);        //低通滤波频率，典型值：0x06(5Hz)
  MPU6050_WriteReg(GYRO_CONFIG,0x18);   //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
  MPU6050_WriteReg(ACCEL_CONFIG,0x01);  //加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
  
  LPLD_SYSTICK_DelayMs(1);
}