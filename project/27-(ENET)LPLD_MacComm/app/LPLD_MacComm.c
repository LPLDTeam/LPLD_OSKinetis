/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_MacComm）-----------------
 * @file LPLD_MacComm.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用ENET模块，实现以太网MAC层数据通讯。
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

/****************************************
 说明：
   *将SER_A扩展板插到RUSH开发板上，通过网线
    连接SER_A板与PC机。
   *将MiniUSB线插入RUSH Kinetis开发板的USB
    插座，并连接至电脑USB接口。
   *通过Wireshake检测PC网卡，观察运行结果。
 ****************************************/

//函数声明
void enet_init(void);
void enet_rx_init(void);
void pit_init(void);
void pit_isr(void);
//变量定义
ENET_InitTypeDef enet_init_struct;
PIT_InitTypeDef pit_init_struct;

//报文类型定义
#define ENET_TYPE_ARP   {0x08, 0x06}
#define ENET_TYPE_IP    {0x08, 0x00}

//本地MAC物理地址定义
uint8  gCfgLoca_MAC[] = {0x22, 0x22, 0x22, 0x00, 0x00, 0x01};
//目的MAC物理地址定义，广播地址
uint8  gCfgDest_MAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//以太帧类型，ARP报文
uint8  gCfgEnet_Type[] = ENET_TYPE_ARP;
//以太帧发送缓冲区
uint8  gTxBuffer[CFG_ENET_TX_BUFFER_SIZE]; 
//以太帧接收标志
uint8  gRecvFlag=0;  

void main (void)
{

  DisableInterrupts;
  
  enet_init();
  
  pit_init();
  
  EnableInterrupts;
  
  while(1)
  {
    //判断是否接收到以太帧
    if(gRecvFlag == 1)
    {
      printf("\tMac Message Received!\r\n");
      gRecvFlag = 0;
    }
  } 
}

/*
 * 初始化ENET模块
 *
 */
void enet_init(void)
{
  //配置ENET模块参数
  enet_init_struct.ENET_MacAddress = gCfgLoca_MAC;      //配置MAC地址
  enet_init_struct.ENET_RxIsr = enet_rx_init;   //配置接收中断函数
  //初始化ENET
  LPLD_ENET_Init(enet_init_struct);
}

/*
 * 以太帧接收中断函数
 *
 */
void enet_rx_init( void )
{  
  gRecvFlag = 1;
}

/*
 * PIT定时中断初始化
 *
 */
void pit_init(void)
{
  pit_init_struct.PIT_Pitx = PIT0;
  pit_init_struct.PIT_PeriodMs = 1000;  //定时1000ms
  pit_init_struct.PIT_Isr = pit_isr;
  LPLD_PIT_Init(pit_init_struct);
  LPLD_PIT_EnableIrq(pit_init_struct);
}

/*
 * 定时中断函数，发送以太广播帧
 *
 */
void pit_isr(void)
{
  uint32 i;
  static uint32 j=0;
  
  //设置以太帧0~5字节为：目的地址
  gTxBuffer[0]  = gCfgDest_MAC[0];
  gTxBuffer[1]  = gCfgDest_MAC[1];
  gTxBuffer[2]  = gCfgDest_MAC[2];
  gTxBuffer[3]  = gCfgDest_MAC[3];
  gTxBuffer[4]  = gCfgDest_MAC[4];
  gTxBuffer[5]  = gCfgDest_MAC[5];
  //设置以太帧1~11字节为：本机地址
  gTxBuffer[6]  = gCfgLoca_MAC[0];
  gTxBuffer[7]  = gCfgLoca_MAC[1];
  gTxBuffer[8]  = gCfgLoca_MAC[2];
  gTxBuffer[9]  = gCfgLoca_MAC[3];
  gTxBuffer[10] = gCfgLoca_MAC[4];
  gTxBuffer[11] = gCfgLoca_MAC[5];
  //设置以太帧12~13字节为：报文类型
  gTxBuffer[12] = gCfgEnet_Type[0];
  gTxBuffer[13] = gCfgEnet_Type[1];
  //设置以太帧14字节以后的数据
  for(i=14; i<CFG_ENET_TX_BUFFER_SIZE; i++)
  {
    gTxBuffer[i] = j++;  
  }
  //发送MAC报文
  LPLD_ENET_MacSend(gTxBuffer, 100);
  printf("Mac Message Send!\r\n");

}
