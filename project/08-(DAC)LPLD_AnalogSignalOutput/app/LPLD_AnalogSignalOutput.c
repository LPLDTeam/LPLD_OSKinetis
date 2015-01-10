/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_AnalogSignalOutput）-----------------
 * @file LPLD_AnalogSignalOutput.c
 * @version 0.2
 * @date 2013-11-4
 * @brief 利用DAC模块，实现模拟电压信号的输出。
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
   *用示波器观察DAC0_OUT引脚的输出波形。
   *可以通过修改DELTA、DELAY的值观察输出变化。
 ****************************************/

#define DELTA   1       //波形增减幅度
#define DELAY   1       //电压输出间隔

//函数声明
void dac_init(void);
void delay(uint16);
//变量定义
DAC_InitTypeDef dac_init_struct;

void main (void)
{
  uint16 i = 0;         //波形输出计数
  int8 delta = DELTA;   //电压增减幅度
  
  //初始化DAC
  dac_init();
  
  while(1)
  {
    if(i==0)
      delta = DELTA;
    else if(i==4095)
      delta = -DELTA;
    
    i += delta;

    //在不使用DAC缓冲区的情况下
    //仅需设置DAC缓冲区0的输出值
    LPLD_DAC_SetBufferDataN(DAC0, i, 1);
    
    delay(DELAY);
  } 
}

/*
 * 初始化DAC模块
 *
 */
void dac_init(void)
{
  //在不使用DAC缓冲区、各种中断DMA的情况下
  //仅需初始化DAC通道即可
  dac_init_struct.DAC_Dacx = DAC0;
  LPLD_DAC_Init(dac_init_struct);
}

/*
 * 延时函数
 */
void delay(uint16 n)
{
  uint16 i;
  while(n--)
  {
    for(i=0; i<10; i++)
    {
      asm("nop");
    }
  }
}
