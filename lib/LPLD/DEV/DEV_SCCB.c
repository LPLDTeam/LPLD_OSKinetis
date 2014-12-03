/**
* @file DEV_SCCB.c
* @version 0.1[By LPLD]
* @date 2013-09-24
* @brief SCCB设备驱动程序
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
#include "DEV_SCCB.h"

static uint8 LPLD_SCCB_Start(void);
static void LPLD_SCCB_Stop(void);
static void LPLD_SCCB_Ack(void);
static void LPLD_SCCB_NoAck(void);
static uint8 LPLD_SCCB_WaitAck(void);
static void LPLD_SCCB_SendByte(uint8);
static uint8 LPLD_SCCB_ReceiveByte(void);
static void LPLD_SCCB_Delay(uint16);

/*
 * LPLD_SCCB_Init
 * 初始化SCCB所需引脚
 */
void LPLD_SCCB_Init(void)
{
  GPIO_InitTypeDef ptb;
  /********用户可修改值 开始***********/
  ptb.GPIO_PTx = PTB;
  ptb.GPIO_Pins = GPIO_Pin0 | GPIO_Pin1;
  /********用户可修改值 结束***********/
  ptb.GPIO_Dir = DIR_OUTPUT;
  ptb.GPIO_Output = OUTPUT_H;
  ptb.GPIO_PinControl = NULL;
  LPLD_GPIO_Init(ptb);
}

/*
 * LPLD_SCCB_WriteReg
 * 写SCCB设备寄存器
 * 
 * 参数:
 *    reg_addr--寄存器地址
 *    data--待写数据 
 *
 * 输出:
 *    1-成功
 *    0-失败
 */
uint8 LPLD_SCCB_WriteReg(uint16 reg_addr , uint8 data)
{		
  if(!LPLD_SCCB_Start())
  {
    return 0;
  }
  LPLD_SCCB_SendByte( SCCB_DEV_ADR );         
  if( !LPLD_SCCB_WaitAck() )
  {
    LPLD_SCCB_Stop(); 
    return 0;
  }
  LPLD_SCCB_SendByte((uint8)(reg_addr & 0x00FF));   
  LPLD_SCCB_WaitAck();	
  LPLD_SCCB_SendByte(data);
  LPLD_SCCB_WaitAck();   
  LPLD_SCCB_Stop(); 
  return 1;
}									 




/******************************************************************************************************************
* 函数名：SCCB_ReadByte
* 描述  ：读取一串数据
* 输入  ：- data: 存放读出数据 	- length: 待读出长度	- reg_addr: 待读出地址		 - DeviceAddress: 器件类型
* 输出  ：返回为:=1成功读入,=0失败
* 注意  ：无        
**********************************************************************************************************************/ 
/*
 * LPLD_SCCB_ReadReg
 * 读SCCB设备寄存器
 * 
 * 参数:
 *    reg_addr--寄存器地址
 *    *data--待存读出数据地址
 *    length--读取长度
 *
 * 输出:
 *    1-成功
 *    0-失败
 */          
uint8 LPLD_SCCB_ReadReg(uint8 reg_addr, uint8* data, uint16 length)
{	
  if(!LPLD_SCCB_Start())
  {
    return 0;
  }
  LPLD_SCCB_SendByte( SCCB_DEV_ADR ); 
  if( !LPLD_SCCB_WaitAck() )
  {
    LPLD_SCCB_Stop(); 
    return 0;
  }
  LPLD_SCCB_SendByte( reg_addr ); 
  LPLD_SCCB_WaitAck();	
  LPLD_SCCB_Stop(); 
  
  if(!LPLD_SCCB_Start())
  {
    return 0;
  }
  LPLD_SCCB_SendByte( SCCB_DEV_ADR + 1 ); 
  if(!LPLD_SCCB_WaitAck())
  {
    LPLD_SCCB_Stop(); 
    return 0;
  }
  while(length)
  {
    *data = LPLD_SCCB_ReceiveByte();
    if(length == 1)
    {
      LPLD_SCCB_NoAck();
    }
    else
    {
      LPLD_SCCB_Ack(); 
    }
    data++;
    length--;
  }
  LPLD_SCCB_Stop();
  return 1;
}

/*
 * LPLD_SCCB_Start
 * SCCB起始信号，内部调用
 */
static uint8 LPLD_SCCB_Start(void)
{
  SCCB_SDA_O=1;
  SCCB_SCL=1;
  SCCB_DELAY();
  
  SCCB_SDA_IN();
  if(!SCCB_SDA_I)
  {
    SCCB_SDA_OUT();	
    return 0;
  }
  SCCB_SDA_OUT();	
  SCCB_SDA_O=0;
  
  SCCB_DELAY();
  
  SCCB_SDA_IN();
  if(SCCB_SDA_I) 
  {
    SCCB_SDA_OUT();
    return 0;
  }
  SCCB_SDA_OUT();
  SCCB_SDA_O=0;

  return 1;
}

/*
 * LPLD_SCCB_Stop
 * SCCB停止信号，内部调用
 */
static void LPLD_SCCB_Stop(void)
{
  SCCB_SCL=0;
  SCCB_SDA_O=0;
  SCCB_DELAY();
  
  SCCB_SCL=1;
  SCCB_SDA_O=1;
  SCCB_DELAY();
}

/*
 * LPLD_SCCB_Stop
 * SCCB应答信号，内部调用
 */
static void LPLD_SCCB_Ack(void)
{	
  SCCB_SCL=0;
  SCCB_DELAY();
  
  SCCB_SDA_O=0;
  SCCB_DELAY();
  
  SCCB_SCL=1;
  SCCB_DELAY();
  
  SCCB_SCL=0;
  SCCB_DELAY();
}

/*
 * LPLD_SCCB_NoAck
 * SCCB无应答信号，内部调用
 */
static void LPLD_SCCB_NoAck(void)
{	
  SCCB_SCL=0;
  SCCB_DELAY();
  SCCB_SDA_O=1;
  SCCB_DELAY();
  SCCB_SCL=1;
  SCCB_DELAY();
  SCCB_SCL=0;
  SCCB_DELAY();
}

/*
 * LPLD_SCCB_WaitAck
 * SCCB等待应答信号，内部调用
 */
static uint8 LPLD_SCCB_WaitAck(void) 	
{
  SCCB_SCL=0;
  SCCB_DELAY();
  SCCB_SDA_O=1;	
  SCCB_DELAY();
  
  SCCB_SCL=1;
  
  SCCB_SDA_IN();
  SCCB_DELAY();
  
  if(SCCB_SDA_I)
  {
    SCCB_SDA_OUT();
    SCCB_SCL=0;
    return 0;
  }
  SCCB_SDA_OUT();
  SCCB_SCL=0;
  return 1;
}

/*
 * LPLD_SCCB_SendByte
 * SCCB发送数据，内部调用
 */
static void LPLD_SCCB_SendByte(uint8 data) 
{
  uint8 i=8;
  while(i--)
  {
    SCCB_SCL=0;
    SCCB_DELAY();
    if(data&0x80)
    {
      SCCB_SDA_O=1; 
    }
    else 
    {
      SCCB_SDA_O=0;   
    }
    data<<=1;
    SCCB_DELAY();
    SCCB_SCL=1;
    SCCB_DELAY();
  }
  SCCB_SCL=0;
}

/*
 * LPLD_SCCB_SendByte
 * SCCB接收数据，内部调用
 */
static uint8 LPLD_SCCB_ReceiveByte(void)  
{ 
  uint8 i=8;
  uint8 ReceiveByte=0;
  
  SCCB_SDA_O=1;	
  SCCB_DELAY();
  
  SCCB_SDA_IN();	
  
  while(i--)
  {
    ReceiveByte<<=1;      
    SCCB_SCL=0;
    SCCB_DELAY();
    
    SCCB_SCL=1;
    SCCB_DELAY();	
    
    if(SCCB_SDA_I)
    {
      ReceiveByte|=0x01;
    }
    
  }
  SCCB_SDA_OUT();
  SCCB_SCL=0;
  
  return ReceiveByte;
}

/*
 * LPLD_SCCB_SendByte
 * SCCB延时函数，内部调用
 */
static void LPLD_SCCB_Delay(uint16 i)
{	
  while(i) 
    i--; 
}


