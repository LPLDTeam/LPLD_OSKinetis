/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_InputCapture）-----------------
 * @file LPLD_InputCapture.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用FTM模块的输入捕捉功能，实现输入脉冲的频率测量。
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
   *将一路脉冲信号接入PTB0引脚，该脉冲可
    引用K60自身生成的PWM信号。
   *本例使用FTM0的PTC1生成PWM信号，因此可
    直接将PTC1连接PTB0用来检测脉冲频率。
   *将MiniUSB线插入RUSH Kinetis开发板的USB
    插座，并连接至电脑USB接口。
   *使用串口调试助手波特率设置为115200
   *使用串口调试助手查看运行结果。
 ****************************************/

//函数声明
void ic_init(void);
void ic_isr(void);
void pwm_init(void);
void delay(uint16);
//变量定义
FTM_InitTypeDef ftm0_init_struct;
FTM_InitTypeDef ftm1_init_struct;
uint32 Freq1;

void main (void)
{
  //生成一路PWM脉冲以供我们测试输入捕获功能
  //PTC1输出的频率为1000Hz，可以修改初始化参数已观察结果
  //也可以注释此句，外接一路PWM到PTB0
  pwm_init();
  
  //初始化输入捕获功能
  ic_init();
  
  while(1)
  {
    printf("Freq = %dHz\r\n", Freq1);
    delay(1000);
  } 
}

/*
 * 初始化FTM0的输入捕获功能
 *
 */
void ic_init(void)
{
  //配置FTM0模块的输入捕获参数
  ftm1_init_struct.FTM_Ftmx = FTM1;      //使能FTM1通道
  ftm1_init_struct.FTM_Mode = FTM_MODE_IC;       //使能输入捕获模式
  ftm1_init_struct.FTM_ClkDiv = FTM_CLK_DIV128;  //计数器频率为总线时钟的128分频
  ftm1_init_struct.FTM_Isr = ic_isr;     //设置中断函数
  //初始化FTM0
  LPLD_FTM_Init(ftm1_init_struct);
  //使能输入捕获对应通道,上升沿捕获进入中断
  LPLD_FTM_IC_Enable(FTM1, FTM_Ch0, PTB0, CAPTURE_RI);
  
  //使能FTM0中断
  LPLD_FTM_EnableIrq(ftm1_init_struct);
}

/*
 * 输入捕获中断
 *
 */
void ic_isr(void)
{
  uint32 cnt;
  //判断是否为FTM1的Ch0通道产生捕获中断
  if(LPLD_FTM_IsCHnF(FTM1, FTM_Ch0))
  {
    //获取FTM1的Ch0通道计数值
    cnt=LPLD_FTM_GetChVal(FTM1, FTM_Ch0);   
    
    //根据总线频率、分频系数、计数值计算脉冲频率
    //脉冲频率=(总线频率/输入捕获分频)/计数值
    Freq1=(g_bus_clock/LPLD_FTM_GetClkDiv(FTM1))/cnt; 
    
    //清空FTM1 COUNTER
    LPLD_FTM_ClearCounter(FTM1);
    //清除输入中断标志
    LPLD_FTM_ClearCHnF(FTM1, FTM_Ch0); 
  }
}


/*
 * 初始化FTM1的PWM输出功能
 *
 */
void pwm_init(void)
{
  ftm0_init_struct.FTM_Ftmx = FTM0;	//使能FTM0通道
  ftm0_init_struct.FTM_Mode = FTM_MODE_PWM;	//使能PWM模式
  ftm0_init_struct.FTM_PwmFreq = 1000;	//PWM频率1000Hz
  
  LPLD_FTM_Init(ftm0_init_struct);
  
  LPLD_FTM_PWM_Enable(FTM0, //使用FTM0
                      FTM_Ch0, //使能Ch0通道
                      5000,     //占空比50.00%
                      PTC1, //使用Ch0通道的PTC1引脚
                      ALIGN_LEFT        //脉宽左对齐
                      ); 
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
