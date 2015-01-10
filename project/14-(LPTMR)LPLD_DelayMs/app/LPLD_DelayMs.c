/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_DelayMs）-----------------
 * @file LPLD_DelayMs.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用LPTMR模块，实现毫秒级精准延时。
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

/********************************************
 * 说明: 
 * 利用K60单片机的LPTMR模块产生Ms级延时
 * 1、通过LPTMR定时中断的方式设置定时时间
 * 2、通过LPTMR查询标志位的方式产生延时
 * 将MiniUSB线插入RUSH Kinetis开发板的USB
 * 插座，并连接至电脑USB接口。
 * 使用串口调试助手波特率设置为115200
 * 使用串口调试助手查看运行结果。
 * 通过改变DELAY_MODE的方式改变中断或者是查询
 * POLLING_DELAY 查询方式
 * INETERRUPT_DELAY 中断方式
 ********************************************/

//通过DELAY_MODE选择是中断定时还是轮询延时
#define DELAY_MODE INETERRUPT_DELAY 
#define POLLING_DELAY     (0)
#define INETERRUPT_DELAY  (1)
LPTMR_InitTypeDef lptmr_init_param;

#if DELAY_MODE == INETERRUPT_DELAY
void lptmr_isr(void);
#endif

void main (void)
{
#if DELAY_MODE == INETERRUPT_DELAY
  //LPTMR_MODE_TIMER低功耗定时器功能
  //使能中断
  lptmr_init_param.LPTMR_Mode = LPTMR_MODE_TIMER;
  lptmr_init_param.LPTMR_PeriodMs = 1000; //1s
  lptmr_init_param.LPTMR_IntEnable = TRUE;
  lptmr_init_param.LPTMR_Isr = lptmr_isr;
  LPLD_LPTMR_Init(lptmr_init_param); 
  LPLD_LPTMR_EnableIrq();
#endif  
  while(1)
  {
#if DELAY_MODE == POLLING_DELAY
    LPLD_LPTMR_DelayMs(2000);
    printf("the interval is 2s\r\n");
#endif
  } 
}

#if DELAY_MODE == INETERRUPT_DELAY
void lptmr_isr(void)
{
  printf("the interval is 1s\r\n");
}
#endif 
