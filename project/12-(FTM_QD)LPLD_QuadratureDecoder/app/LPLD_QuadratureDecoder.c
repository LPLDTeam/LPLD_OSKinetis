/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_QuadratureDecoder）-----------------
 * @file LPLD_QuadratureDecoder.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用FTM模块的正交解码功能，实现编码器的正反转测速。
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
   *分别将编码器的AB相信号接入PTB0、PTB1引脚
   *将MiniUSB线插入RUSH Kinetis开发板的USB
    插座，并连接至电脑USB接口。
   *使用串口调试助手波特率设置为115200
   *使用串口调试助手查看运行结果。
   *使编码器产生正反转，查看运行结果。
 ****************************************/

//函数声明
void qd_init(void);
void pit_init(void);
void pit_isr(void);
void delay(uint16);
//变量定义
FTM_InitTypeDef ftm_init_struct;
PIT_InitTypeDef pit_init_struct;
int32 qd_result=0;

void main (void)
{
  
  //初始化正交解码功能
  qd_init();
  //初始化PIT定时中断
  //用来定时读取正交解码的计数结果
  pit_init();

  while(1)
  {
    printf("QD Counter = %d\r\n", qd_result);
    delay(1000);
  } 
}

/*
 * 初始化FTM1的正交解码功能
 *
 */
void qd_init(void)
{
  //配置正交解码功能参数
  ftm_init_struct.FTM_Ftmx = FTM1;              //只有FTM1和FTM2有正交解码功能
  ftm_init_struct.FTM_Mode = FTM_MODE_QD;       //正交解码功能
  ftm_init_struct.FTM_QdMode = QD_MODE_PHAB;    //AB相输入模式
  //初始化FTM
  LPLD_FTM_Init(ftm_init_struct);
  //使能AB相输入通道
  //PTB0引脚接A相输入、PTB1引脚接B相输入
  LPLD_FTM_QD_Enable(FTM1, PTB0, PTB1);
}

/*
 * 初始化PIT定时器
 *
 */
void pit_init(void)
{
  //配置定时周期中断参数
  pit_init_struct.PIT_Pitx = PIT0;
  pit_init_struct.PIT_PeriodMs = 20;    //20毫秒产生一起中断
  pit_init_struct.PIT_Isr = pit_isr;
  //初始化PIT
  LPLD_PIT_Init(pit_init_struct);
  //使能PIT定时中断
  LPLD_PIT_EnableIrq(pit_init_struct);
}

/*
 * PIT中断函数
 */
void pit_isr(void)
{
  //获取FTM1的正交解码计数值
  qd_result = LPLD_FTM_GetCounter(FTM1);
  //清空计数器
  LPLD_FTM_ClearCounter(FTM1);
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
