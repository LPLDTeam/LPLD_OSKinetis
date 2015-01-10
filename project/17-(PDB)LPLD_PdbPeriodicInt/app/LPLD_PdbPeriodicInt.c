/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_PdbPeriodicInt）-----------------
 * @file LPLD_PdbPeriodicInt.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用PDB模块，实现延时周期定时中断。
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
   *将MiniUSB线插入RUSH Kinetis开发板的USB
    插座，并连接至电脑USB接口。
   *使用串口调试助手波特率设置为115200
   *使用串口调试助手查看运行结果。
 ****************************************/

//函数声明
void pdb_init(void);
void pdb_isr(void);
//变量定义
PDB_InitTypeDef pdb_init_struct;

void main (void)
{
  //初始化PDB延时中断
  pdb_init();   
  //打印此句方便查看延时效果
  printf("Waiting for 200ms, and then:\r\n");

  while(1)
  {

  } 
}

/*
 * 初始化PDB
 *
 */
void pdb_init(void)
{
  //配置PDB延时定时中断参数
  pdb_init_struct.PDB_CounterPeriodMs = 1000;   //计数器溢出周期1000毫秒
  pdb_init_struct.PDB_TriggerInputSourceSel = TRIGGER_SOFTWARE; //触发源为软件触发
  pdb_init_struct.PDB_ContinuousModeEnable = TRUE;      //连续工作模式
  pdb_init_struct.PDB_DelayMs = 200;    //中断延时时间200毫秒
  pdb_init_struct.PDB_IntEnable = TRUE; //使能延时中断
  pdb_init_struct.PDB_Isr = pdb_isr;    //中断函数设置
  //初始化PDB
  LPLD_PDB_Init(pdb_init_struct);
  //使能PDB中断
  LPLD_PDB_EnableIrq();
  //软件触发
  LPLD_PDB_SoftwareTrigger();
}

/*
 * PDB中断函数
 *
 */
void pdb_isr(void)
{
  //此句打印时间为PDB初始化后的200ms
  printf("This is a PDB delay interrupt!\r\n");
}