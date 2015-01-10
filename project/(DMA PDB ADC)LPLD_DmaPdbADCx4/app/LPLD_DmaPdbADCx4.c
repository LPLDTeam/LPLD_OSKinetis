/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_DmaPdbADCx4）-----------------
 * @file LPLD_DmaPdbADCx4.c
 * @version 0.1[By LPLD]
 * @date 2013-10-30
 * @brief 综合利用ADC、PDB、DMA模块，实现四通道AD自动连续采集，无需CPU干预，采集结果随时从内存读取
 *
 * 版权所有:北京拉普兰德电子技术有限公司
 * 官网 http://www.lpld.cn
 * 交流论坛 http://bbs.lpld.cn
 * mail:support@lpld.cn
 * 硬件平台:  RUSH Kinetis开发板
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
    作为模拟量调节器，其余3个AD输入口为C0,B4,B5。
   *将MiniUSB线插入RUSH Kinetis开发板的USB
    插座，并连接至电脑USB接口。
   *使用串口调试助手波特率设置为115200
   *使用串口调试助手查看运行结果。
   *通过调节电位器的旋钮查看运行结果,也可将
    模拟量输入到C0,B4,B5观察其他通道变化。
 ****************************************/

//定义各个模块的初始化结构体
ADC_InitTypeDef adc0_init_struct;
ADC_InitTypeDef adc1_init_struct;
DMA_InitTypeDef dma0_init_struct;
DMA_InitTypeDef dma1_init_struct;
PDB_InitTypeDef pdb_init_struct;

//定义结果保存数组，分别存储ADC0-A、ADC0-B、ADC1-A、ADC1-B
int16 Result[4]={0,0,0,0};

//简单延时函数
void delay(void);

/********************************************************************/
void main (void)
{
  //**********************************************
  //Step 1.配置ADC0和ADC1的参数
  //ADC0-A
  adc0_init_struct.ADC_Adcx = ADC0;
  adc0_init_struct.ADC_BitMode = SE_12BIT;      //12位精度
  adc0_init_struct.ADC_CalEnable = TRUE;        //使能初始化校验
  adc0_init_struct.ADC_HwAvgSel = HW_4AVG;      //使能4次硬件校准
  adc0_init_struct.ADC_HwTrgCfg = HW_TRGA;      //配置A组为硬件触发
  adc0_init_struct.ADC_DmaEnable = TRUE;        //使能DMA请求
  LPLD_ADC_Init(adc0_init_struct);      //初始化ADC0-A组通道
  //ADC0-B，只需配置不同参数的成员变量
  adc0_init_struct.ADC_HwTrgCfg = HW_TRGB;      //配置B组为硬件触发
  LPLD_ADC_Init(adc0_init_struct);      //初始化ADC0-B组通道
  //使能ADC0的通道DAD1、AD14的引脚复用功能
  LPLD_ADC_Chn_Enable(ADC0, DAD1);
  LPLD_ADC_Chn_Enable(ADC0, AD14);
  //使能ADC0的相关转换通道
  LPLD_ADC_EnableConversion(ADC0, DAD1, 0, FALSE);
  LPLD_ADC_EnableConversion(ADC0, AD14, 1, FALSE);
  
  //ADC1A
  adc1_init_struct.ADC_Adcx = ADC1;
  adc1_init_struct.ADC_BitMode = SE_12BIT;
  adc1_init_struct.ADC_CalEnable = TRUE;
  adc1_init_struct.ADC_HwAvgSel = HW_4AVG;
  adc1_init_struct.ADC_HwTrgCfg = HW_TRGA;
  adc1_init_struct.ADC_DmaEnable = TRUE;
  LPLD_ADC_Init(adc1_init_struct);
  //ADC1B
  adc1_init_struct.ADC_HwTrgCfg = HW_TRGB;
  LPLD_ADC_Init(adc1_init_struct);
  LPLD_ADC_Chn_Enable(ADC1, AD10);
  LPLD_ADC_Chn_Enable(ADC1, AD11);
  LPLD_ADC_EnableConversion(ADC1, AD10, 0, FALSE);
  LPLD_ADC_EnableConversion(ADC1, AD11, 1, FALSE);
    
  //**********************************************
  //Step 2.配置DMA CH0和DMA CH1，分别处理ADC0和ADC1的DMA请求
  //DMA CH0
  dma0_init_struct.DMA_CHx = DMA_CH0;           //使用Ch0通道
  dma0_init_struct.DMA_Req = ADC0_DMAREQ;       //DMA请求源为ADC0
  dma0_init_struct.DMA_MajorLoopCnt = 2;        //主循环计数2次,因为要循环ADC0的AB两组通道
  dma0_init_struct.DMA_MinorByteCnt = 2;        //次循环传输字节计数（由于ADC采样为12位，因此传输2字节）
  dma0_init_struct.DMA_SourceAddr = (uint32)&(ADC0->R[0]);       //源地址：ADC0结果寄存器A地址
  dma0_init_struct.DMA_SourceDataSize = DMA_SRC_16BIT;   //源地址传输数据宽度16位
  dma0_init_struct.DMA_SourceAddrOffset = 4;    //源地址偏移为4个字节，因为ADC0->R寄存为32位宽，A组传输完成后移动到B组
  dma0_init_struct.DMA_LastSourceAddrAdj = -8;  //主循环最后调节地址为-8个字节，因为主循环为2次计数，因此地址偏移了8个字节
  dma0_init_struct.DMA_DestAddr = (uint32)&Result;       //目的地址，即定义的结果保存数组头地址
  dma0_init_struct.DMA_DestDataSize = DMA_DST_16BIT;     //目的地址传输数据宽度16位
  dma0_init_struct.DMA_DestAddrOffset = 2;      //目的地址偏移为2个字节，因为Result为16位变量
  dma0_init_struct.DMA_LastDestAddrAdj = -4;    //目的地址最后调节地址为-4个字节
  dma0_init_struct.DMA_AutoDisableReq = FALSE;   //禁用自动禁用请求，即不受主循环计数计数限制
  //初始化DMA
  LPLD_DMA_Init(dma0_init_struct);
  //使能DMA请求
  LPLD_DMA_EnableReq(DMA_CH0);
  
  //DMA CH1，配置基本相同
  dma1_init_struct.DMA_CHx = DMA_CH1;   
  dma1_init_struct.DMA_Req = ADC1_DMAREQ;       
  dma1_init_struct.DMA_MajorLoopCnt = 2;        
  dma1_init_struct.DMA_MinorByteCnt = 2; 
  dma1_init_struct.DMA_SourceAddr = (uint32)&(ADC1->R[0]);       
  dma1_init_struct.DMA_SourceDataSize = DMA_SRC_16BIT;  
  dma1_init_struct.DMA_SourceAddrOffset = 4;
  dma1_init_struct.DMA_LastSourceAddrAdj = -8;
  dma1_init_struct.DMA_DestAddr = (uint32)&Result+4;       //目的地址，由于数组中头2个元素存ADC0的结果，因此要偏移4个字节取第3个元素的地址
  dma1_init_struct.DMA_DestDataSize = DMA_DST_16BIT;  
  dma1_init_struct.DMA_DestAddrOffset = 2;
  dma1_init_struct.DMA_LastDestAddrAdj = -4;
  dma1_init_struct.DMA_AutoDisableReq = FALSE;   
  //初始化DMA
  LPLD_DMA_Init(dma1_init_struct);
  //使能DMA请求
  LPLD_DMA_EnableReq(DMA_CH1);
    
  //**********************************************
  //Step 3.配置PDB，用于触发ADC
  pdb_init_struct.PDB_CounterPeriodMs = 100;    //PDB计数器周期，这个决定了4个通道每采集一次的间隔
  pdb_init_struct.PDB_LoadModeSel = LOADMODE_0;
  pdb_init_struct.PDB_ContinuousModeEnable = TRUE;      //使能连续工作模式，即只需要开始触发一次，以后PDB就会连续工作
  pdb_init_struct.PDB_TriggerInputSourceSel = TRIGGER_SOFTWARE; //软件触发模式，即不需要用其他模块触发PDB工作
  //初始化PDB
  LPLD_PDB_Init(pdb_init_struct);
  //配置PDB预触发功能
  //使能ADC0-A组的预触发功能
  LPLD_PDB_AdcTriggerCfg(ADC0, PRETRIG_EN_A|PRETRIG_DLY_A, 0);  
  //使能ADC0-B组的预触发功能，并使用Back to Back模式
  LPLD_PDB_AdcTriggerCfg(ADC0, PRETRIG_BB_B|PRETRIG_EN_B|PRETRIG_DLY_B, 0);
  //使能ADC1-A组的预触发功能，并使用Back to Back模式
  LPLD_PDB_AdcTriggerCfg(ADC1, PRETRIG_BB_A|PRETRIG_EN_A|PRETRIG_DLY_A, 0);
  //使能ADC1-B组的预触发功能，并使用Back to Back模式
  LPLD_PDB_AdcTriggerCfg(ADC1, PRETRIG_BB_B|PRETRIG_EN_B|PRETRIG_DLY_B, 0);
  //软件触发PDB开始工作
  LPLD_PDB_SoftwareTrigger();

  while(1)
  {
    delay();
    printf("ADC0_RA=%d\r\n", Result[0]);
    printf("  ADC0_RB=%d\r\n", Result[1]);
    printf("    ADC1_RA=%d\r\n", Result[2]);
    printf("      ADC1_RB=%d\r\n", Result[3]);
  } 
}


void delay()
{
  uint32 i=0,j=0;
  while(i<5000)
  {
    i++;
    j=0;
    while(j<1000)
      j++;
  }
}
