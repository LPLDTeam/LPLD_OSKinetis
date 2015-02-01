/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_ServoControl）-----------------
 * @file LPLD_ServoControl.c
 * @version 0.2
 * @date 2013-10-30
 * @brief 利用FTM模块的PWM输出功能，实现舵机的控制。
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
   CPU_MK60DZ10 & CPU_MK60D10
   *将PTC1引脚连接至舵机的信号线
   CPU_MK60F12 & CPU_MK60F15
   *将PTD0引脚连接至舵机的信号线
    观察舵机的运动状态
   *舵机控制信号脉宽范围0.5ms~2.5ms
    即(-90°~+90°)
 ****************************************/

//函数声明
void pwm_init(void);
void delay(uint16);
uint32 angle_to_period(int8);
//变量定义
FTM_InitTypeDef ftm_init_struct;

void main (void)
{

  //初始化PWM
  pwm_init();
  
  delay(1000);
  //初始化延时后改变角度为45度
#if defined(CPU_MK60DZ10) || defined(CPU_MK60D10)
  LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch0, angle_to_period(45));
#elif defined(CPU_MK60F12) || defined(CPU_MK60F15)
  LPLD_FTM_PWM_ChangeDuty(FTM2, FTM_Ch0, angle_to_period(45));
#endif 
  while(1)
  {
  } 
}

/*
 * 初始化FTM的PWM输出功能
 *
 */
void pwm_init(void)
{
#if defined(CPU_MK60DZ10) || defined(CPU_MK60D10)
  ftm_init_struct.FTM_Ftmx = FTM0;	//使能FTM0通道
  ftm_init_struct.FTM_Mode = FTM_MODE_PWM;	//使能PWM模式
  ftm_init_struct.FTM_PwmFreq = 50;	//PWM频率50Hz
  
  LPLD_FTM_Init(ftm_init_struct);
  
  LPLD_FTM_PWM_Enable(FTM0, //使用FTM0
                      FTM_Ch0, //使能Ch0通道
                      angle_to_period(0), //初始化角度0度
                      PTC1, //使用Ch0通道的PTC1引脚
                      ALIGN_LEFT        //脉宽左对齐
                      );  
#elif defined(CPU_MK60F12) || defined(CPU_MK60F15)  
  ftm_init_struct.FTM_Ftmx = FTM2;	//使能FTM0通道
  ftm_init_struct.FTM_Mode = FTM_MODE_PWM;	//使能PWM模式
  ftm_init_struct.FTM_PwmFreq = 50;	//PWM频率50Hz
  
  LPLD_FTM_Init(ftm_init_struct);
  
  LPLD_FTM_PWM_Enable(FTM2, //使用FTM0
                      FTM_Ch0, //使能Ch0通道
                      angle_to_period(0), //初始化角度0度
                      PTD0, //使用Ch0通道的PTD0引脚
                      ALIGN_LEFT        //脉宽左对齐
                      );
#endif
}

/*
 * 将舵机的角度值转化为PWM的频率
 * deg--角度值(-90°~+90°)
 *   |__-90~+90
 *
 */
uint32 angle_to_period(int8 deg)
{
  uint32 pulse_width = 500+2000*(90+deg)/180;
  return (uint32)(10000*pulse_width/20000);
}

/*
 * 延时函数
 */
void delay(uint16 n)
{
  uint16 i;
  while(n--)
  {
    for(i=0; i<5000; i++)
    {
      asm("nop");
    }
  }
}