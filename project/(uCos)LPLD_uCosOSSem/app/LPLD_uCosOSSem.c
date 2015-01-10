/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_uCosOSSem）-----------------
 * @file LPLD_uCosOSSem.c
 * @version 0.0
 * @date 2013-9-29
 * @brief 关于该工程的描述
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
 * 将MiniUSB线插入RUSH Kinetis开发板Card上的USB
   插座，并连接至电脑USB接口。
 * App_TaskStart任务会因为等待信号量Task_Start而挂起
 * 当按下RUSH Kinetis开发板S1按键，会触发按键中断并发送
   信号量
 * 此时App_TaskStart任务接收到信号量，转为执行态
   由串口发送数据
 ********************************************/

//UCOS
//创建任务堆栈
OS_STK App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];		 
//建立信号量
OS_EVENT *Task_Start; 

GPIO_InitTypeDef Key_S1;

void App_TaskStart(void  *p_arg);
void key_isr(void);
/********************************************************************/
void  main (void)
{
#if (defined(UCOS_II))
  INT8U err;
  
  //初始化UCOS，启动系统内核
  OSInit();      
  
  printf("uCOS_Kernel_Init \r\n");
  
  //创建初始任务
  OSTaskCreateExt((void (*)(void *)) App_TaskStart,         
                  (void           *) 0,
                  (OS_STK         *)&App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                  (INT8U           ) APP_CFG_TASK_START_PRIO,
                  (INT16U          ) APP_CFG_TASK_START_PRIO,
                  (OS_STK         *)&App_TaskStartStk[0],
                  (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                  (void           *) 0,
                  (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
  //任务添加名字，以便在监视窗口可以看到
#if (OS_TASK_NAME_EN > 0)
  OSTaskNameSet(APP_CFG_TASK_START_PRIO, "App_TaskStart", &err);
#endif    
  //开始多任务处理，进入内核进行调度
  OSStart();                                                 
#endif  
  while(1);
}

//起始任务
void  App_TaskStart (void *p_arg)
{
 (void)p_arg;
 
  INT8U err;
  //初始化OS时钟
  SystemTickInit(); 

#if (OS_TASK_STAT_EN > 0)
  OSStatInit();                                              
#endif    
  Key_S1.GPIO_PTx  = PTB;
  Key_S1.GPIO_Pins = GPIO_Pin6;
  Key_S1.GPIO_PinControl = INPUT_PULL_UP | IRQC_FA;
  Key_S1.GPIO_Dir  = DIR_INPUT;
  Key_S1.GPIO_Isr  = key_isr;
  
  LPLD_GPIO_Init(Key_S1); 
  LPLD_GPIO_EnableIrq(Key_S1);
  //创建信号量，设置初值为0
  //返回信号量句柄
  Task_Start = OSSemCreate(0); 
  while (TRUE) 
  {                                          
    //等待信号量，如果信号量未到，该任务处于挂起状态
    OSSemPend(Task_Start,0,&err);
    
    if(err == OS_ERR_NONE )
    {
      printf("App_TaskStart:\tApp_TaskStart \r\n");
    }
  }
}

void key_isr(void)
{
  if(LPLD_GPIO_IsPinxExt(PORTB,GPIO_Pin6))
  {
    //发送信号量
    OSSemPost(Task_Start);
  }
}

