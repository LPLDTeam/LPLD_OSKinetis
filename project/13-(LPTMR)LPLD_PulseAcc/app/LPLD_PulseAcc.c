/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_PulseAcc）-----------------
 * @file LPLD_PulseAcc.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用LPTMR模块，实现脉冲累加功能。
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
 * 利用K60单片机的FTM模块产生100Khz脉冲波
 * 将脉冲波输入到LPTMR 脉冲累加通道
 * LPTMR进行脉冲累并通过串口打印输出脉冲累加结果
 * 硬件配置说明:
 * K60单片机引脚       
 * FTM    -PTC1  -----> PTC5   LPTMR ALT2 
 * 使用串口调试助手波特率设置为115200
 * 使用串口调试助手查看运行结果。
 ********************************************/

#define PIT_TIMER_PERIOD 10000 //10ms
                               
FTM_InitTypeDef ftm_init_param;
PIT_InitTypeDef pit_init_param;
LPTMR_InitTypeDef lptmr_init_param;
uint16_t GetFreq;
uint16_t GetCnt;

void pit_isr0();
/********************************************************************/

void main (void)
{
  //注意：采集的频率必须大于100HZ
  //初始化FTM
  ftm_init_param.FTM_Ftmx = FTM0;
  ftm_init_param.FTM_PwmFreq = 100000;//100Khz
  ftm_init_param.FTM_Mode = FTM_MODE_PWM;
  //初始化PIT
  pit_init_param.PIT_Pitx = PIT0;
  pit_init_param.PIT_PeriodUs = PIT_TIMER_PERIOD;//10ms 0.1khz
  pit_init_param.PIT_Isr = pit_isr0; 
  //LPTMR_MODE_PLACC脉冲累加模式
  //LPTMR_ALT2的输入口为PTC5
  //禁用中断
  //无中断函数 
  lptmr_init_param.LPTMR_Mode = LPTMR_MODE_PLACC;
  lptmr_init_param.LPTMR_PluseAccInput = LPTMR_ALT2;
  lptmr_init_param.LPTMR_Isr = NULL;
  //FTM,PIT,LPTMR初始化
  LPLD_PIT_Init(pit_init_param);
  LPLD_PIT_EnableIrq(pit_init_param); //使能PIT中断
  LPLD_FTM_Init(ftm_init_param);
  LPLD_LPTMR_Init(lptmr_init_param); 
  LPLD_FTM_PWM_Enable(FTM0,FTM_Ch0,5000,PTC1,ALIGN_LEFT);
 
  GetFreq=0;
  GetCnt=0;
  
  while(1)
  {   
  }
}


void pit_isr0()
{
    static uint32_t temp=0;
    
    if(GetCnt>99) //进行100次累加，累加次数越高，精度越高，但实时性降低
    {
       GetFreq=temp/100; //取平均
       printf("the PulseAcc:%d hz \r\n",GetFreq*100);
       temp=0;
       GetCnt=0;
    }
    else
    {
       temp+=LPLD_LPTMR_GetPulseAcc();
       GetCnt++;
    }
    
    LPLD_LPTMR_ResetCounter();       //复位LPTMR0 counter 
    LPLD_LPTMR_Init(lptmr_init_param);
}

