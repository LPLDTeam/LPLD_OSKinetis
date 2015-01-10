/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_PeriodicInterrupt）-----------------
 * @file LPLD_PeriodicInterrupt.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用PIT模块，实现周期定时中断。
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
   *使用串口调试助手波特率设置为115200
   *使用串口调试助手查看运行结果。
   *通过修改PIT0、PIT1的定时周期观察输出变化。
 ****************************************/

//函数声明
void pit_init(void);
void pit0_isr(void);
void pit1_isr(void);
//变量定义
PIT_InitTypeDef pit0_init_struct;
PIT_InitTypeDef pit1_init_struct;

void main (void)
{
  //初始化PIT
  pit_init();

  while(1)
  {

  } 
}

/*
 * 初始化PIT0和PIT1通道
 *
 */
void pit_init(void)
{
  //配置PIT0参数
  pit0_init_struct.PIT_Pitx = PIT0;
  pit0_init_struct.PIT_PeriodS = 2;     //定时周期2秒
  pit0_init_struct.PIT_Isr = pit0_isr;  //设置中断函数
  //初始化PIT0
  LPLD_PIT_Init(pit0_init_struct);  
    
  //配置PIT1参数
  pit1_init_struct.PIT_Pitx = PIT1;
  pit1_init_struct.PIT_PeriodMs = 1000; //定时周期1000毫秒
  pit1_init_struct.PIT_Isr = pit1_isr;  //设置中断函数
  //初始化PIT1
  LPLD_PIT_Init(pit1_init_struct);
  
  //使能PIT0和PIT1
  LPLD_PIT_EnableIrq(pit0_init_struct);
  LPLD_PIT_EnableIrq(pit1_init_struct);
}

/*
 * PIT0中断函数
 *
 */
void pit0_isr(void)
{
  printf("PIT0 Interrupt!\r\n");
}

/*
 * PIT1中断函数
 *
 */
void pit1_isr(void)
{
  printf("\tPIT1 Interrupt!\r\n");
}