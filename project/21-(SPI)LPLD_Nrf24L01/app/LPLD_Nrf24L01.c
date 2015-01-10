/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_Nrf24L01）-----------------
 * @file LPLD_Nrf24L01.c
 * @version 0.2
 * @date 2014-2-10
 * @brief 利用SPI模块，通过Nrf24L01无线模块进行数据收发。
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
#include "DEV_Nrf24L01.h"

/********************************************
 * 说明: 
 * 硬件配置说明:
 * K60单片机引脚        NRF24L01引脚
 * 按键S1 -PTB6
 *     INT    -PTC1  -----> IRQ
 *     CE     -PTB0  -----> CE
 *     CS     -PTA14 -----> CSN
 *     SCK    -PTA15 -----> SCK
 *     MOSI   -PTA16 -----> MOSI
 *     MISO   -PTA17 <----- MISO
 *
 * 发送端：通过按键触发NRF24L01发送指令
 * 接收端：通过PTC1中断，接收数据
 * 将MiniUSB线插入RUSH Kinetis开发板的USB
   插座，并连接至电脑USB接口。
 * 使用串口调试助手波特率设置为115200
 * 使用串口调试助手查看运行结果。
 * 通过修改NRF24L01_WORKMODE改变发送和接收方式
 ********************************************/

//选择编译方式 决定当前例程中NRF24L01的工作方式
#define NRF24L01_RX_WORKMODE      (0) //发送程序
#define NRF24L01_TX_WORKMODE      (1) //接收程序
#define NRF24L01_WORKMODE         NRF24L01_TX_WORKMODE 

uint8 rx_buf[NRF24L01_PLOAD_LEN] = {0x00};     //接收缓冲区
uint8 tx_buf[NRF24L01_PLOAD_LEN] = {0x00};     //发送缓冲区

uint8 Flag_Smitte = 0; //发送标志，
GPIO_InitTypeDef gpio_init_struct;
//中断函数声明
void portc_handle(void);
void portb_handle(void);

void main (void)
{
  int i; 
  LPLD_Nrf24L01_Init();
#if NRF24L01_WORKMODE == NRF24L01_TX_WORKMODE 
  gpio_init_struct.GPIO_PTx = PTB;        //PORTB
  gpio_init_struct.GPIO_Pins = GPIO_Pin6; //引脚6
  gpio_init_struct.GPIO_Dir = DIR_INPUT;  //输入
  gpio_init_struct.GPIO_PinControl = INPUT_PULL_UP|IRQC_FA;  //内部上拉|下降沿中断
  gpio_init_struct.GPIO_Isr = portb_handle;        //中断函数
  
  LPLD_GPIO_Init(gpio_init_struct);
  LPLD_GPIO_EnableIrq(gpio_init_struct); //内部下拉,使能开发板上的按键 S1 PORTB6
  LPLD_Nrf24L01_TxModeInit();
#else
  gpio_init_struct.GPIO_PTx = PTC;        //PORTC
  gpio_init_struct.GPIO_Pins = GPIO_Pin1; //引脚1
  gpio_init_struct.GPIO_Dir = DIR_INPUT;  //输入
  gpio_init_struct.GPIO_PinControl = INPUT_PULL_DOWN|IRQC_FA;  //内部下拉|下降沿中断
  gpio_init_struct.GPIO_Isr = portc_handle;      //中断函数
    
  LPLD_GPIO_Init(gpio_init_struct);
  LPLD_GPIO_EnableIrq(gpio_init_struct); //内部下拉,使能PORTC1作为NRF24L01的IRQ中断
  LPLD_Nrf24L01_RxModeInit();
#endif
   
  while(1)
  {
#if NRF24L01_WORKMODE == NRF24L01_TX_WORKMODE
    if(Flag_Smitte)
    {
      for(i = 0 ;i <32 ;i++)//设置发送的32个字节数据
      {
        tx_buf[i] = i;
      }
      if(LPLD_Nrf24L01_SendFrame(tx_buf,NRF24L01_PLOAD_LEN))
      {
        printf("Nrf24l01 Send Succeed!!\r\n");
      }
      else
      {
        printf("Nrf24l01 Send Error!!\r\n");
      }
      Flag_Smitte = 0;
    }
#endif
    
  }
}

#if NRF24L01_WORKMODE == NRF24L01_TX_WORKMODE
//按键中断
void portb_handle()
{
  Flag_Smitte = 1; //设置发送标志
}
#endif

#if NRF24L01_WORKMODE == NRF24L01_RX_WORKMODE
//nrf24l01接收中断
void portc_handle()
{  
  printf("Nrf24L01 rev Succeed!!\r\n"); 
  LPLD_Nrf24L01_RecvFrame(rx_buf, NRF24L01_PLOAD_LEN);
  LPLD_UART_PutCharArr(UART5, (char*)rx_buf, NRF24L01_PLOAD_LEN);//通过串口5打印输出值
  printf("\r\n");
}
#endif
/********************************************************************/
