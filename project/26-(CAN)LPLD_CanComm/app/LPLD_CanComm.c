/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_CanComm）-----------------
 * @file LPLD_CanComm.c
 * @version 0.3
 * @date 2014-3-9
 * @brief 通过CAN模块进行数据帧收发。
 *
 * 版权所有:北京拉普兰德电子技术有限公司
 * http://www.lpld.cn
 * mail:support@lpld.cn
 * 硬件平台:  LPLD K60 Card / LPLD K60 Nano
 *
 * 本工程基于"拉普兰德K60底层库V3"开发，
 * 所有开源代码均在"lib"文件夹下，用户不必更改该目录下代码，
 * 所有用户工程需保存在"project"文件夹下，以工程名定义文件夹名，
 * 底层库使用方法见相关文档。 
 *
 */
#include "common.h"

/*******************************************************
* 说明：
* FlexCAN 测试例程，通过该例程测试FlexCAN的工作情况
* 被测试例程定义了发送模式和接收模式两种
* 通过宏定义FLEX_CAN_MODE 来区分发送还是接收功能
* 如果需要发送功能只需定义#define FLEX_CAN_MODE REV_MODE
* 如果需要接收功能只需定义#define FLEX_CAN_MODE SEND_MODE
* 单片机的内核必须定义为96Mhz，硬件总线时钟频率48Mhz
* #define CORE_CLK_MHZ        PLL_96   
********************************************************/

#define SEND_MODE     (1)
#define REV_MODE      (0)
#ifndef FLEX_CAN_MODE
#define FLEX_CAN_MODE REV_MODE
#endif

/*
定义接收回掉函数
如下所示：
*/
void can_rx_isr(void);
/*
本例程采用按键触发CAN发送函数
如下所示：
*/
void key_isr(void);
void key_delay(int time);
GPIO_InitTypeDef key_init_t;

/*
首先需要定义结构体对象
本例程用到的CAN初始化结构体CAN_InitTypeDef
CAN消息缓冲区初始化结构体CAN_MSGOBJ_InitTypeDef

CAN_InitTypeDef需要定义结构体对象flexcan_init_param_t
CAN_MSGOBJ_InitTypeDef已经在HW_CAN.c中进行定义，故
用extern引用即可
如下所示：
*/
CAN_InitTypeDef flexcan_init_param_t;

/*
需要CAN帧的结构体，用于存储发送和接收的消息
如下所示：
*/
#if  FLEX_CAN_MODE != REV_MODE 
uint8 send_buffer[8]= "HiLPLD\r\n";//要发送的数据
CAN_MessageFormat_TypeDef can_tx_message; //用于存储发送消息
#else
CAN_MessageFormat_TypeDef can_rx_message; //用于存储接收消息
#endif
  
void main (void)
{

#if  FLEX_CAN_MODE != REV_MODE 
/*
在发送模式下
初始化发送结构体
如下所示：设置对方的ID、要发送的数据、数据长度
*/  
  can_tx_message.CAN_MsgID = 0x1AAAAAAA; //0x1AAAAAAA 29位扩展帧;
  memcpy(can_tx_message.CAN_MsgDataBuffer,send_buffer,sizeof(send_buffer));
  can_tx_message.CAN_MsgDataLength = sizeof(send_buffer);
#endif   

/*
初始化CAN_InitTypeDef，用于设定CAN模块的工作状态
如下所示：设置1Mbps波特率，通道为CAN1，全局匹配模式，发送引脚为PTC17、接收为PTC16
*/    
  flexcan_init_param_t.CAN_BaudRate = CAN_BAUD_RATE_1MBPS;
  flexcan_init_param_t.CAN_Canx  = CAN1;
  flexcan_init_param_t.CAN_RxMaskMode = CAN_MSGOBJ_GLOBAL_MASKING;
  flexcan_init_param_t.CAN_RxPin = PTC16;
  flexcan_init_param_t.CAN_TxPin = PTC17; 
  
  LPLD_CAN_Init(flexcan_init_param_t);
  LPLD_CAN_EnableIrq(flexcan_init_param_t);
#if  FLEX_CAN_MODE == REV_MODE
/*
在接收模式下
初始化接收结构体CAN_MSGOBJ_InitTypeDef
处理接收的数据
  接收的数据存储在 can_rx_message 中
CAN_MessageFormat_TypeDef can_rx_message; //用于存储接收消息
初始化参数如下所示：
*/   
  flexcan_msgobj_param_t[MSG_NUM_0].CAN_Canx           = CAN1;     //如果用到0只需去掉注释
  flexcan_msgobj_param_t[MSG_NUM_0].CAN_MsgNum         = MSG_NUM_0;//Msg0
  flexcan_msgobj_param_t[MSG_NUM_0].CAN_MsgDirection   = CAN_MSGOBJ_DIR_RX; //接收模式
  flexcan_msgobj_param_t[MSG_NUM_0].CAN_MsgIdLength    = CAN_MSGOBJ_ID_EXT; //29位扩展帧
  flexcan_msgobj_param_t[MSG_NUM_0].CAN_MsgInterrupt   = TRUE;              //使能该Msg0中断
  flexcan_msgobj_param_t[MSG_NUM_0].CAN_MsgSRR         = 0;                 //禁止SRR
  flexcan_msgobj_param_t[MSG_NUM_0].CAN_MsgRTR         = 0;                 //禁止RTR
  flexcan_msgobj_param_t[MSG_NUM_0].CAN_Isr            = can_rx_isr;      //设置中断回掉函数
  
  //LPLD_CAN_InitMessageObject(flexcan_msgobj_param_t[MSG_NUM_0],0x7E0);//ID设置成相同
  LPLD_CAN_InitMessageObject(flexcan_msgobj_param_t[MSG_NUM_0],0x1AAAAAAA);//ID设置成相同
#else 
/*
在发送模式下
利用MSG_NUM_1作为发送缓冲
如果成功发送将触发中断，可以设置中断回掉函数，在中断回掉函数中再次发送另一帧数据
在这里没有设置发送中断回到函数
*/ 
  flexcan_msgobj_param_t[MSG_NUM_1].CAN_Canx           = CAN1;//如果用到0只需去掉注释
  flexcan_msgobj_param_t[MSG_NUM_1].CAN_MsgNum         = MSG_NUM_1;
  flexcan_msgobj_param_t[MSG_NUM_1].CAN_MsgDirection   = CAN_MSGOBJ_DIR_TX;//发送模式
  flexcan_msgobj_param_t[MSG_NUM_1].CAN_MsgIdLength    = CAN_MSGOBJ_ID_EXT;//29位扩展帧
  flexcan_msgobj_param_t[MSG_NUM_1].CAN_MsgInterrupt   = TRUE;//使能该Msg1中断
  flexcan_msgobj_param_t[MSG_NUM_1].CAN_MsgSRR         = 0;
  flexcan_msgobj_param_t[MSG_NUM_1].CAN_MsgRTR         = 0;
  LPLD_CAN_InitMessageObject(flexcan_msgobj_param_t[MSG_NUM_1],NULL); //使能MSG_NUM_1中断
#endif 

//使能按键中断
#if  FLEX_CAN_MODE != REV_MODE  
  key_init_t.GPIO_PTx        = PTB;
  key_init_t.GPIO_Pins       = GPIO_Pin7|GPIO_Pin6;
  key_init_t.GPIO_Dir        = DIR_INPUT;
  key_init_t.GPIO_PinControl = INPUT_PULL_UP|OUTPUT_SR_FAST|INPUT_PF_DIS|IRQC_FA;
  key_init_t.GPIO_Isr        = key_isr;
  
  LPLD_GPIO_Init(key_init_t);
  LPLD_GPIO_EnableIrq(key_init_t);
#endif
  
  while(1)
  {

  } 
}

#if  FLEX_CAN_MODE == REV_MODE 
/*
在接收模式下
如果成功接收后，会进入如下的中断回掉函数中执行
将数据存储在can_rx_message
*/ 
void can_rx_isr(void)
{
  LPLD_CAN_ReceivedMessage(flexcan_msgobj_param_t[MSG_NUM_0],&can_rx_message);
  LPLD_UART_PutCharArr(UART5,(int8*)can_rx_message.CAN_MsgDataBuffer,can_rx_message.CAN_MsgDataLength);
}
#endif

#if  FLEX_CAN_MODE != REV_MODE 
void key_isr()
{
  if(LPLD_GPIO_IsPinxExt(PORTB,GPIO_Pin6))
  {
    //利用按键触发发送函数
    LPLD_CAN_TransmitMessage(flexcan_msgobj_param_t[MSG_NUM_1],&can_tx_message);
    key_delay(1000);//去抖
  }
  if(LPLD_GPIO_IsPinxExt(PORTB,GPIO_Pin7))
  {
    asm("nop");
  }
}
void key_delay(int time)
{
  while(time--)
  {
    asm("nop");
  }
}
#endif
/********************************************************************/
