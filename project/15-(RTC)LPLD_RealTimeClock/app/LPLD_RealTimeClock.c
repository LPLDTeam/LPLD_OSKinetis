/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_AlarmClock）-----------------
 * @file LPLD_AlarmClock.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用RTC模块，实现闹钟功能。
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
#include "TimeStamp.h"
/********************************************
 * 说明: 
 * RTC 每10s定时报警，并产生报警中断
 * 将MiniUSB线插入RUSH Kinetis开发板的USB
 * 插座，并连接至电脑USB接口。
 * 使用串口调试助手波特率设置为115200
 * 使用串口调试助手查看运行结果。
 ********************************************/
RTC_InitTypeDef rtc_init_structure;
TimeStamp_FormatTypeDef time;

void rtc_isr(void);
/********************************************************************/
void main (void)
{
  uint32 temp;
  
  //开启RTC SIM 时钟，通过判断TIF获知
  //当前RTC是否发生VBAT POR或者 Software Reset
  //如果没有发生，RTC正常计数   
  if(LPLD_RTC_IsRunning() == 0) 
  {
    printf("RTC init\r\n");
    
    LPLD_AutoInitTimeStamp(&time);

    rtc_init_structure.RTC_Seconds = LPLD_DateTimeToTimeStamp(&time);
    rtc_init_structure.RTC_InvalidIsr = rtc_isr;
    rtc_init_structure.RTC_InvalidIntEnable = TRUE;
    LPLD_RTC_Init(rtc_init_structure);
    LPLD_RTC_EnableIrq();
  }
  else //RTC正常计数
  {
    printf("RTC run\r\n");
  }
  
  while(1)
  {
    temp = LPLD_RTC_GetRealTime();
    LPLD_TimeStampToDateTime(temp,&time);
    printf("%4d-%02d-%02d %02d:%02d:%02d\r\n", time.year, time.month, time.day, time.hour, time.minute, time.second);
    LPLD_LPTMR_DelayMs(1000);
  } 
}

void rtc_isr()
{
  printf("RTC VBAT POR\r\n"); //产生电池掉电中断
}


