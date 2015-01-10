/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_AnalogSampleDIFF）-----------------
 * @file LPLD_AnalogSampleDIFF.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用ADC模块的差分采集功能，实现模拟电压的差分采集。
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
   *使用RUSH Kinetis开发板上的电位器R7
    作为模拟量调节器。
   *将MiniUSB线插入RUSH Kinetis开发板的USB
    插座，并连接至电脑USB接口。
   *使用串口调试助手波特率设置为115200
   *使用串口调试助手查看运行结果。
   *通过调节电位器的旋钮查看运行结果。
   *可使用"LPLD ADC采样时间计算器"计算ADC的采样速率。
 ****************************************/

//函数声明
void adc_init(void);
void delay(uint16);
//变量定义
ADC_InitTypeDef adc_init_struct;

void main (void)
{
  int16 result;
  
  //初始化ADC
  adc_init();
  
  while(1)
  {
    result = (int16)LPLD_ADC_Get(ADC0, DAD1);
    printf("DAD1 Diff result = %d\r\n", result);
    delay(5000);
  } 
}

/*
 * 初始化ADC及其通道
 *
 */
void adc_init(void)
{
  //配置ADC采样参数
  adc_init_struct.ADC_Adcx = ADC0;
  adc_init_struct.ADC_DiffMode = ADC_DIFF;      //差分采集
  adc_init_struct.ADC_BitMode = DIFF_13BIT;     //差分13位精度
  adc_init_struct.ADC_SampleTimeCfg = SAMTIME_SHORT;    //短采样时间
  adc_init_struct.ADC_HwAvgSel = HW_4AVG;       //4次硬件平均
  adc_init_struct.ADC_CalEnable = TRUE; //使能初始化校验
  //初始化ADC
  LPLD_ADC_Init(adc_init_struct);
  //为了保证复用功能不被占用，最好使能相应ADC通道
  //RUSH开发板的电位器使用DAD1通道进行采集
  //差分DAD1通道的引脚为(ADC0_DP1和ADC0_DM1)
  LPLD_ADC_Chn_Enable(ADC0, DAD1); 
}

/*
 * 延时函数
 */
void delay(uint16 n)
{
  uint16 i;
  while(n--)
  {
    for(i=0; i<1000; i++)
    {
      asm("nop");
    }
  }
}