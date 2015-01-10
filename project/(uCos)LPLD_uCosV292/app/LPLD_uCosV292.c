/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_uCosV292）-----------------
 * @file LPLD_uCosV292.c
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
 * 创建两个UCOS任务, App_TaskStart和 App_Task1
 * App_TaskStart任务2ms释放一次CPU使用权，
 * App_Task1任务1ms释放一次CPU使用权，
 * 通过串口调试助手查看两个任务的切换状态
 * 将MiniUSB线插入RUSH Kinetis开发板的USB
 * 插座，并连接至电脑USB接口。
 * 使用串口调试助手波特率设置为115200
 * 使用串口调试助手查看运行结果。
 ********************************************/

//UCOS
//创建任务堆栈
OS_STK App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];	//创建开始任务堆栈
OS_STK App_Task1_Stk[APP_CFG_TASK_START_STK_SIZE];      //创建开始任务1堆栈

//建立信号量
OS_EVENT *Task_Start; 

void App_TaskStart(void  *p_arg);
void App_Task1 (void *p_arg);
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
  OSTaskCreate((void (*)(void *)) App_Task1,     //创建任务1                                      
                (void*) 0,
                (OS_STK*)&App_Task1_Stk[APP_CFG_TASK_START_STK_SIZE - 1],
                (INT8U) APP_TASK1_PRIO
                );
   //任务添加名字，以便在监视窗口可以看到
#if (OS_TASK_NAME_EN > 0)
  OSTaskNameSet(APP_TASK1_PRIO, "APP_TASK1", &err);
#endif 
  while (TRUE) 
  {                                          
    OSTimeDlyHMSM(0, 0,2,0);
    printf("App_TaskStart:\tApp_TaskStart \r\n");
  }
}

void  App_Task1 (void *p_arg)
{
  (void)p_arg;
  
  printf("App_Task1 !!\n");
  
  while(TRUE)
  {
    OSTimeDlyHMSM(0, 0,1,0);
    printf("App_Task1:\tApp_Task1 \r\n");
  }
}

