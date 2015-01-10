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

/********************************************
 * 说明: 
 * RTC 每10s定时报警，并产生报警中断
 * 将MiniUSB线插入RUSH Kinetis开发板的USB
 * 插座，并连接至电脑USB接口。
 * 使用串口调试助手波特率设置为115200
 * 使用串口调试助手查看运行结果。
 ********************************************/

void rtc_isr();

RTC_InitTypeDef rtc_init_structure;

/********************************************************************/
void main (void)
{
  rtc_init_structure.RTC_Seconds = 0;
  rtc_init_structure.RTC_AlarmTime = 10; //设置报警时间，第10s时报警
  rtc_init_structure.RTC_AlarmIsr = rtc_isr;
  rtc_init_structure.RTC_AlarmIntEnable = TRUE; //使能报警中断
  
  LPLD_RTC_Init(rtc_init_structure); //初始化RTC
  LPLD_RTC_EnableIrq(); //开启中断
  while(1)
  {

  } 
}

void rtc_isr()
{
  rtc_init_structure.RTC_AlarmTime += 10;
  printf("the AlarmTime is : 10s\r\n"); 
  LPLD_RTC_SetAlarmTime(rtc_init_structure.RTC_AlarmTime); //重新设置报警时间
}

