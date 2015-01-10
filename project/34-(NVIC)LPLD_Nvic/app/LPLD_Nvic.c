/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_Nvic）-----------------
 * @file LPLD_Nvic.c
 * @version 0.1
 * @date 2013-12-1
 * @brief 测试通过NVIC模块设置中断的抢占式优先级
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

//如果测试抢占式中断，则定义为1，否则为0
#define PREEMPTION      1

PIT_InitTypeDef pit0_init_struct;
PIT_InitTypeDef pit1_init_struct;
NVIC_InitTypeDef nvic_init_struct;

void pit0_isr(void);
void pit1_isr(void);

void main (void)
{
#if(PREEMPTION == 1)
  //配置PIT0的NVIC分组
  nvic_init_struct.NVIC_IRQChannel = PIT0_IRQn;
  nvic_init_struct.NVIC_IRQChannelGroupPriority = NVIC_PriorityGroup_1;
  nvic_init_struct.NVIC_IRQChannelPreemptionPriority = 0;
  LPLD_NVIC_Init(nvic_init_struct);
  //配置PIT1的NVIC分组
  nvic_init_struct.NVIC_IRQChannel = PIT1_IRQn;
  nvic_init_struct.NVIC_IRQChannelGroupPriority = NVIC_PriorityGroup_1;
  nvic_init_struct.NVIC_IRQChannelPreemptionPriority = 1;
  LPLD_NVIC_Init(nvic_init_struct);
#endif
  
  //配置PIT0
  pit0_init_struct.PIT_Pitx = PIT0;
  pit0_init_struct.PIT_PeriodMs = 500;
  pit0_init_struct.PIT_Isr = pit0_isr;
  LPLD_PIT_Init(pit0_init_struct);
  //配置PIT1
  pit1_init_struct.PIT_Pitx = PIT1;
  pit1_init_struct.PIT_PeriodMs = 1000;
  pit1_init_struct.PIT_Isr = pit1_isr;
  LPLD_PIT_Init(pit1_init_struct);
  
  LPLD_PIT_EnableIrq(pit0_init_struct);
  LPLD_PIT_EnableIrq(pit1_init_struct);
  
  while(1)
  {
  } 
}

void pit0_isr(void)
{
  printf(">>PIT0 enter int.\r\n");
  LPLD_SYSTICK_DelayMs(50);
  printf("<<PIT0 leave int.\r\n");
}

void pit1_isr(void)
{
  printf("  >>PIT1 enter int.\r\n");
  LPLD_SYSTICK_DelayMs(150);
  LPLD_SYSTICK_DelayMs(150);
  LPLD_SYSTICK_DelayMs(150);
  LPLD_SYSTICK_DelayMs(150);
  printf("  <<PIT1 leave int.\r\n");
}

