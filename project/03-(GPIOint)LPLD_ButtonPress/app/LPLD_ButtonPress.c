/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_ButtonPress）-----------------
 * @file LPLD_ButtonPress.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用GPIO模块及其中断，判断按键动作。
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
   *使用RUSH Kinetis开发板上的按键S1和S2
    作为控制按键。
   *将MiniUSB线插入RUSH Kinetis开发板的USB
    插座，并连接至电脑USB接口。
   *使用串口调试助手波特率设置为115200
   *使用串口调试助手查看运行结果。
   *通过修改INT改变中断和查询方式。
 ****************************************/

//若使用中断方式则为1，若使查询方式则为0
#define INT  1   

//函数声明
void init_gpio(void);
void delay(void);
void portb_isr(void);
//变量声明
GPIO_InitTypeDef gpio_init_struct;

void main (void)
{
  
  //初始化 GPIO
  init_gpio();
  
  while(1)
  {
#if(INT!=1) //如果为查询方式
    if(PTB6_I == 0)
    {
      //去抖
      delay();
      if(PTB6_I==0)
      {
        printf("Button1-PTB6 Pressed!\r\n");
      }
      //直到按键松开再运行
      while(PTB6_I==0);
    }
    if(PTB7_I == 0)
    {
      //去抖
      delay();
      if(PTB7_I==0)
      {
        printf("Button2-PTB7 Pressed!\r\n");
      }
      //直到按键松开再运行
      while(PTB7_I==0);
    }
#endif
  } 
}

/*
 * 初始化连接按键的GPIO引脚
 *
 */
void init_gpio()
{

#if(INT) //如果为中断方式
  // 配置 PTB7、PTB6 为GPIO功能,输入,内部上拉，上升沿产生中断
  gpio_init_struct.GPIO_PTx = PTB;      //PORTB
  gpio_init_struct.GPIO_Pins = GPIO_Pin6|GPIO_Pin7;     //引脚6、7
  gpio_init_struct.GPIO_Dir = DIR_INPUT;        //输入
  gpio_init_struct.GPIO_PinControl = INPUT_PULL_UP|IRQC_FA;     //内部上拉|上升沿中断
  gpio_init_struct.GPIO_Isr = portb_isr;        //中断函数
  LPLD_GPIO_Init(gpio_init_struct);
  //使能中断
  LPLD_GPIO_EnableIrq(gpio_init_struct);
#else //如果为查询方式
  // 配置 PTB7、PTB6 为GPIO功能,输入,内部上拉，不产生中断
  gpio_init_struct.GPIO_PTx = PTB;      //PORTB
  gpio_init_struct.GPIO_Pins = GPIO_Pin6|GPIO_Pin7;     //引脚6、7
  gpio_init_struct.GPIO_Dir = DIR_INPUT;        //输入
  gpio_init_struct.GPIO_PinControl = INPUT_PULL_UP|IRQC_DIS;     //内部上拉|不产生中断
  LPLD_GPIO_Init(gpio_init_struct);
#endif
  
}

/*
 * PortB中断回调函数
 *
 */
void portb_isr()
{
  //如果PTB6产生中断
  if(LPLD_GPIO_IsPinxExt(PORTB, GPIO_Pin6))
  {
    //去抖
    delay();
    if(PTB6_I==0)
    {
      printf("Button1-PTB6 Interrupt!\r\n");
    }
  }
  
  //如果PTB7产生中断
  if(LPLD_GPIO_IsPinxExt(PORTB, GPIO_Pin7))
  {
    //去抖
    delay();
    if(PTB7_I==0)
    {
      printf("Button2-PTB7 Interrupt!\r\n");
    }
  }
}

/*
 * 延时一段时间
 */
void delay()
{
  uint16 i, n;
  for(i=0;i<30000;i++)
  {
    for(n=0;n<50;n++)
    {
      asm("nop");
    }
  }
}