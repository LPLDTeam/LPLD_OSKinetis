/**
* --------------基于"拉普兰德K60底层库V3"的工程（LPLD_OV7670）-----------------
* @file LPLD_OV7670.c
* @version 0.1
* @date 2013-9-29
* @brief 利用DMA模块，实现数字摄像头OV7670的图像读取。
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
#include "DEV_SCCB.h"

/****************************************
说明：
*将MiniUSB线插入RUSH Kinetis开发板的USB
插座，并连接至电脑USB接口。
*将OV7670上位机波特率设置为115200
*使用上位机查看运行结果
****************************************/

//图像宽度
#define H 250
//图像高度
#define V 200
#define PHOTO_SIZE H*V

void porta_isr(void);
void portb_isr(void);
void gpio_init(void);
void dma_init(void);

uint8 ov7670_init();
void delay(void);

GPIO_InitTypeDef pta_init;
GPIO_InitTypeDef ptb_init;
GPIO_InitTypeDef pte_init;
GPIO_InitTypeDef ptd_init;
DMA_InitTypeDef dma_init_struct;

uint8 Pix_Data[PHOTO_SIZE];       //采集50行 200列的图像数据  
uint32 V_Cnt;                     //行采集计数
uint8 Is_DispPhoto;               //图像发送标志

/********************************************************************/
void main (void)
{
 
  //关闭全局中断
  DisableInterrupts;
  
  ov7670_init();
  delay();
  
  V_Cnt=0;          //行计数
  Is_DispPhoto=0;   //显示图像
  
  gpio_init();
  dma_init();
  
  //使能PORTA、PORTB中断
  enable_irq(PORTB_IRQn);
  enable_irq(PORTA_IRQn);
  //使能全局中断
  EnableInterrupts;
  
  
  while(1)
  {
    
    if(Is_DispPhoto==1)
    {   
      Is_DispPhoto = 0;
      //显示或发送图像数据
      //可根据上位机软件自行修改此处
      
      //清PORTA中断标志
      LPLD_GPIO_ClearIntFlag(PORTA);
      //使能PORTA中断
      enable_irq(PORTA_IRQn);
      
    } 
    
  } 
}

void gpio_init()
{
  //OV数据口初始化：PTD8~PTD15
  ptd_init.GPIO_PTx = PTD;
  ptd_init.GPIO_Dir = DIR_INPUT;
  ptd_init.GPIO_Pins = GPIO_Pin8_15;
  ptd_init.GPIO_PinControl = IRQC_DIS | INPUT_PULL_DIS;
  LPLD_GPIO_Init(ptd_init);
  
  //OV行信号接口初始化：PTB3-H
  ptb_init.GPIO_PTx = PTB;
  ptb_init.GPIO_Dir = DIR_INPUT;
  ptb_init.GPIO_Pins = GPIO_Pin3;
  ptb_init.GPIO_PinControl = IRQC_RI|INPUT_PULL_DOWN;
  ptb_init.GPIO_Isr = portb_isr;
  LPLD_GPIO_Init(ptb_init); 
  
  //OV场信号接口初始化：PTA5-V
  pta_init.GPIO_PTx = PTA;
  pta_init.GPIO_Dir = DIR_INPUT;
  pta_init.GPIO_Pins = GPIO_Pin5;
  pta_init.GPIO_PinControl = IRQC_RI|INPUT_PULL_DOWN;
  pta_init.GPIO_Isr = porta_isr;
  LPLD_GPIO_Init(pta_init); 
  
  //OV PCLK信号接口初始化：PTE6-PCLK
  pte_init.GPIO_PTx = PTE;
  pte_init.GPIO_Pins = GPIO_Pin6;
  pte_init.GPIO_Dir = DIR_INPUT;
  pte_init.GPIO_PinControl = IRQC_DMARI | INPUT_PULL_DIS;
  LPLD_GPIO_Init(pte_init); 
  
}

void dma_init()
{
  //DMA参数配置
  dma_init_struct.DMA_CHx = DMA_CH0;    //CH0通道
  dma_init_struct.DMA_Req = PORTE_DMAREQ;       //PORTE为请求源
  dma_init_struct.DMA_MajorLoopCnt = H; //主循环计数值：行采集点数，宽度
  dma_init_struct.DMA_MinorByteCnt = 1; //次循环字节计数：每次读入1字节
  dma_init_struct.DMA_SourceAddr = (uint32)&PTD->PDIR+1;        //源地址：PTD8~15
  dma_init_struct.DMA_DestAddr = (uint32)Pix_Data;      //目的地址：存放图像的数组
  dma_init_struct.DMA_DestAddrOffset = 1;       //目的地址偏移：每次读入增加1
  dma_init_struct.DMA_AutoDisableReq = TRUE;    //自动禁用请求
  //初始化DMA
  LPLD_DMA_Init(dma_init_struct);
}


void porta_isr()
{
  if(LPLD_GPIO_IsPinxExt(PORTA, GPIO_Pin5))
  {
    //检测到场开始信号，加载目的地址
    LPLD_DMA_LoadDstAddr(DMA_CH0, Pix_Data);
    //清行中断标志，防止进入无效行中断
    LPLD_GPIO_ClearIntFlag(PORTB);
    enable_irq(PORTB_IRQn);
  }
  
}
void portb_isr()
{
  if(LPLD_GPIO_IsPinxExt(PORTB, GPIO_Pin3))
  {
    //检测到行开始信号，使能DMA请求
    if(V_Cnt<V){
      LPLD_DMA_EnableReq(DMA_CH0);  
      V_Cnt++; 
    }
    //行数采集已满，关闭中断
    else
    {
      //关GPIO中断 
      disable_irq(PORTA_IRQn);
      disable_irq(PORTB_IRQn);
      Is_DispPhoto = 1;//可以显示图像
      V_Cnt=0;  
    }
  }
}


uint8 ov7670_init(void)
{
  uint16 i = 0;
  uint8 device_id = 0;
  
  LPLD_SCCB_Init();
  
  delay();
  
  //复位OV7670寄存器
  while( 0 == LPLD_SCCB_WriteReg(0x12, 0x80 )) 
  {
    i++;
    if(i == 500)
    {
      return 0 ;
    }    
  }
  delay();
  
  //读取设备ID
  while( 0 == LPLD_SCCB_ReadReg(0x1C, &device_id, 1));	
  
  //如果为OV7670
  if(device_id == 0x7F)
  {  
    //while( 0 == LPLD_SCCB_WriteReg(0x1E, 0x21) );
    while( 0 == LPLD_SCCB_WriteReg(0x3E, 0x19) );	
    while( 0 == LPLD_SCCB_WriteReg(0x73, 0x09) );	
    //while( 0 == LPLD_SCCB_WriteReg(0x15, 0x20) );	
  }
  else
  {
    return 0;
  }
  
  return 1;
}

/*
 * 延时一段时间
 */
void delay()
{
  uint16 i, n;
  for(i=0;i<30000;i++)
  {
    for(n=0;n<200;n++)
    {
      asm("nop");
    }
  }
}