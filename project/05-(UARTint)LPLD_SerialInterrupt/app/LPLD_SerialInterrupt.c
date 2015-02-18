/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_SerialInterrupt）-----------------
 * @file LPLD_SerialInterrupt.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用UART模块及其中断功能，实现串口的数据收发。
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
   *将MiniUSB线插入RUSH Kinetis开发板的USB
    插座，并连接至电脑USB接口。
   *使用串口调试助手波特率设置为9600
   *使用串口调试助手查看运行结果
 ****************************************/
#define USE                (0)
#define UN_USE             (1) 
#define USE_DMA_Tx_Rx      (UN_USE)
#define USE_FIFO_Tx_Rx     (UN_USE)

/****************************************
 说明：
   1 \#define USE_DMA_Tx_Rx  (UN_USE)
      #define USE_FIFO_Tx_Rx (UN_USE)
        此时设置成为串口接收中断, 不使能FIFO 和 DMA 功能，
        PC发送的数据会触发接收中断，在中断中将接收数据再发送到PC
   
   2 \#define USE_DMA_Tx_Rx  (UN_USE)
      #define USE_FIFO_Tx_Rx (USE)
        此时设置发送,接收为FIFO方式
                 __
                  /| TX_FIFO:|Dataword3|
                 /           |Dataword2|<----uart->watermark
                /            |Dataword1|_______TX_BUS
    data-->uart->D         
               |\           |Dataword3| _______RX_BUS
                 \          |Dataword2|<----uart->watermark
                  \ RX_FIFO:|Dataword1| 
        发送，接收的FIFO深度需要通过读取 UART_PFIFO_TXFIFOSIZE 和 
        UART_PFIFO_TXFIFOSIZE 来获得；
        设置发送，接收的FIFO watermark可以在满足条件的情况下触发中断或者DMA请求
            //如果Rx FIFO buffer中的DataWords 大于等于 UART_FIFOWaterMark
            //将产生中断或者DMA信号，取决C2_RIE和C5_RDMAS的状态
            //如果Tx FIFO buffer中的DataWords 小于等于 UART_FIFOWaterMark
            //将产生中断或者DMA信号，取决C2_TIE和C5_TDMAS的状态
        测试时PC发送的数据会根据UART_FIFOWaterMark的情况触发接收中断，
        在中断中将接收数据再发送到PC
   
   3 \#define USE_DMA_Tx_Rx  (USE)
      #define USE_FIFO_Tx_Rx (USE)
        此时设置发送,接收为FIFO方式,
        Rx FIFO 会根据与watermark的关系触发DMA请求，将从PC接收的数据存储到RAMbuffer中。
        当DMA计数器清零后，产生DMA完成中断，在该中断中使能 DMA ch1.
        Tx FIFO 会根据与watermark的关系触发DMA请求，通过DMA ch1将RAMbuffer中的数据发送给PC
   
   4 \#define USE_DMA_Tx_Rx  (USE)
      #define USE_FIFO_Tx_Rx (UN_USE)
        当接收到数据后会触发DMA请求，将从PC接收的数据存储到RAMbuffer中。
        当DMA计数器清零后，产生DMA完成中断，在该中断中使能 DMA ch1.
        UART会触发DMA请求，通过DMA ch1将RAMbuffer中的数据发送给PC。
 ****************************************/
   
//函数声明  
void uart_init(void);
void uart_isr(void);
void dma_isr(void);
//变量声明
UART_InitTypeDef uart0_init_struct;
#if USE_DMA_Tx_Rx == USE
DMA_InitTypeDef uart_rev_dma_init_struct;
DMA_InitTypeDef uart_trn_dma_init_struct;
uint8 recv_ram_buf[8];
#endif
void main (void)
{
  uart_init();
 
  while(1)
  {

  } 
}

/*
 * 初始化UART5模块
 *
 */
void uart_init(void)
{
  uart0_init_struct.UART_Uartx = UART0;  //使用UART0
  uart0_init_struct.UART_BaudRate = 9600; //设置波特率9600
  uart0_init_struct.UART_RxPin = PTA15;  //接收引脚为PTE9
  uart0_init_struct.UART_TxPin = PTA14;  //发送引脚为PTE8

#if USE_FIFO_Tx_Rx == USE
  uart0_init_struct.RxFIFO.FIFO_Enable = TRUE;
  uart0_init_struct.TxFIFO.FIFO_Enable = TRUE;
  uart0_init_struct.RxFIFO.FIFO_WaterMark = 1;
  uart0_init_struct.RxFIFO.FIFO_BufFlow_IntEnable = TRUE;
  uart0_init_struct.TxFIFO.FIFO_WaterMark = 1;
  uart0_init_struct.TxFIFO.FIFO_BufFlow_IntEnable = TRUE;
#endif
  
#if USE_DMA_Tx_Rx == USE
  uart0_init_struct.UART_RxDMAEnable = TRUE;    //使能接收DMA
  uart0_init_struct.UART_TxDMAEnable = TRUE;    //使能发送DMA
  //配置接收DMA
  uart_rev_dma_init_struct.DMA_CHx = DMA_CH0;                     //选择DMA CH0作为通道
  uart_rev_dma_init_struct.DMA_MajorLoopCnt   = sizeof(recv_ram_buf); //设置计数器长度为数组长度
  uart_rev_dma_init_struct.DMA_MinorByteCnt   = 1;                //设置每次的长度为byte
  uart_rev_dma_init_struct.DMA_MajorCompleteIntEnable = TRUE;     //设置DMA 计数器清零中断
  uart_rev_dma_init_struct.DMA_Req            = UART0_REV_DMAREQ; //设置DMA 请求为 UART rev
  uart_rev_dma_init_struct.DMA_DestAddr       = (uint32)recv_ram_buf; //设置目的地址为 recv_ram_buf
  uart_rev_dma_init_struct.DMA_DestAddrOffset = 1;                //每一次DMA请求，地址加1
  uart_rev_dma_init_struct.DMA_DestDataSize   = DMA_DST_8BIT;     //设置目的长度为byte
  uart_rev_dma_init_struct.DMA_SourceAddr     = (uint32)&UART0->D;//设置UART D为源地址
  uart_rev_dma_init_struct.DMA_SourceDataSize = DMA_SRC_8BIT;     //设置源长度为byte
  uart_rev_dma_init_struct.DMA_AutoDisableReq = FALSE;            //循环接收
  uart_rev_dma_init_struct.DMA_Isr = dma_isr;
  //配置发送DMA
  uart_trn_dma_init_struct.DMA_CHx = DMA_CH1;                     //选择DMA CH0作为通道
  uart_trn_dma_init_struct.DMA_MajorLoopCnt   = sizeof(recv_ram_buf); //设置计数器长度为数组长度
  uart_trn_dma_init_struct.DMA_MinorByteCnt   = 1;                //设置每次的长度为byte
  uart_trn_dma_init_struct.DMA_MajorCompleteIntEnable = FALSE;    //清空DMA 计数器清零中断
  uart_trn_dma_init_struct.DMA_Req            = UART0_TRAN_DMAREQ;//设置DMA 请求为 UART trn
  uart_trn_dma_init_struct.DMA_SourceAddr     = (uint32)recv_ram_buf; //设置目的地址为 recv_ram_buf
  uart_trn_dma_init_struct.DMA_SourceAddrOffset = 1;               //每一次DMA请求，地址加1
  uart_trn_dma_init_struct.DMA_SourceDataSize = DMA_SRC_8BIT;     //设置源长度为byte
  uart_trn_dma_init_struct.DMA_DestAddr       = (uint32)&UART0->D;//设置UART D为源地址
  uart_trn_dma_init_struct.DMA_DestDataSize   = DMA_DST_8BIT;     //设置目的长度为byte
  uart_trn_dma_init_struct.DMA_AutoDisableReq = TRUE;             //禁止循环发送
  
  LPLD_DMA_Init(uart_rev_dma_init_struct); //初始化DMA 接收
  LPLD_DMA_Init(uart_trn_dma_init_struct); //初始化DMA 发送
  LPLD_DMA_EnableReq(DMA_CH0);         //使能DMA请求

  LPLD_DMA_EnableIrq(uart_rev_dma_init_struct);   //使能DMA CH0中断
#endif
  
  uart0_init_struct.UART_RxIntEnable = TRUE;    //使能接收中断
  uart0_init_struct.UART_TxIntEnable = TRUE;    //使能发送中断
  uart0_init_struct.UART_RxIsr = uart_isr;      //设置接收中断函数
  LPLD_UART_Init(uart0_init_struct);            //初始化UART
  
#if USE_DMA_Tx_Rx != USE
  //使能UART中断
  LPLD_UART_EnableIrq(uart0_init_struct);
#endif
}

/*
 * UART0接收中断函数
 *
 */
void uart_isr(void)
{
  int8 recv;
  recv = LPLD_UART_GetChar(UART0);
  LPLD_UART_PutChar(UART0, recv);
}

/*
 * DAM CH0中断函数
 *
 */
void dma_isr(void)
{
  LPLD_DMA_EnableReq(DMA_CH1);         //使能DMA请求
}
