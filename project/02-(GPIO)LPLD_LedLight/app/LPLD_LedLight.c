/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_LedLight）-----------------
 * @file LPLD_LedLight.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用GPIO模块，控制LED小灯的亮灭。
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
   *通过修改CARD宏定义适应不同核心板
 ****************************************/

//若使用拉普兰德K60 Card则为1，若使用K60 Nano则为0
#define CARD  1   

//函数声明
void init_gpio(void);
void delay(void);
//变量声明
GPIO_InitTypeDef gpio_init_struct;

void main (void)
{
  int i;

  //初始化 GPIO
  init_gpio();
  
#if(CARD) 
  i=8;
#else
  i=4;
#endif
  
  while(1)
  {
#if(CARD)
    //D1至D8依次触发点亮、熄灭
    LPLD_GPIO_Toggle_b(PTD, i);
    i+=1;
    if(i==16)
       i=8;
#else
    //D1至D4依次触发点亮、熄灭
    LPLD_GPIO_Toggle_b(PTA, i);
    i+=2;
    if(i==12)
       i=4;
#endif
    delay();
  }
}

/*
 * 初始化连接LED灯的GPIO引脚
 *
 */
void init_gpio()
{
  
#if(CARD)
  // 配置 PTD8~PTD15 为GPIO功能,输出,高电平，禁用中断
  gpio_init_struct.GPIO_PTx = PTD;
  gpio_init_struct.GPIO_Pins = GPIO_Pin8_15;
  gpio_init_struct.GPIO_Dir = DIR_OUTPUT;
  gpio_init_struct.GPIO_Output = OUTPUT_H;
  gpio_init_struct.GPIO_PinControl = IRQC_DIS;
  LPLD_GPIO_Init(gpio_init_struct);
#else
  gpio_init_struct.GPIO_PTx = PTA;
  gpio_init_struct.GPIO_Pins = GPIO_Pin4|GPIO_Pin6|GPIO_Pin8|GPIO_Pin10;
  gpio_init_struct.GPIO_Dir = DIR_OUTPUT;
  gpio_init_struct.GPIO_Output = OUTPUT_H;
  gpio_init_struct.GPIO_PinControl = IRQC_DIS;
  LPLD_GPIO_Init(gpio_init_struct);
#endif
  
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

