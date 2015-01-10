/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_TouchPad）-----------------
 * @file LPLD_TouchPad.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用TSI模块，判断触摸面板的触控操作。
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
   *用手指触摸开发板的O和X区域查看运行结果。
 ****************************************/

//触摸有效计数值，用于识别有效触摸
#define PRESS_CNT       (50)
//触摸按键编号
#define PAD_X   0
#define PAD_O   1

//函数声明
void tsi_init(void);
void tsi_isr(void);
//变量定义
TSI_InitTypeDef tsi_init_struct;
uint8 press_counter[2] = {PRESS_CNT, PRESS_CNT};

void main (void)
{
  
  //初始化TSI
  tsi_init();

  while(1)
  {

  } 
}

/*
 * 初始化TSI
 *
 */
void tsi_init(void)
{
  //配置TSI初始化参数
  tsi_init_struct.TSI_Chs = TSI_Ch7 | TSI_Ch8;  //X、O触摸按键分别为7、8通道
  tsi_init_struct.TSI_ScanTriggerMode = TSI_SCAN_PERIOD;        //周期扫描模式
  tsi_init_struct.TSI_IsInitSelfCal = TRUE;     //启用初始化时自校验功能
  tsi_init_struct.TSI_EndScanOrOutRangeInt = TSI_ENDOFDCAN_INT; //使能扫描结束中断
  tsi_init_struct.TSI_EndScanIsr = tsi_isr;     //设置中断函数
  //初始化TSI
  LPLD_TSI_Init(tsi_init_struct);
  //使能TSI中断
  LPLD_TSI_EnableIrq();
}

/*
 * TSI周期扫描中断
 *
 */
void tsi_isr()
{
  uint8 valid_touch = 0;
  
  //判断7通道是否超过触摸阈值
  if(LPLD_TSI_IsChxTouched(7))
  {
    //触摸有效计数器自减
    press_counter[PAD_X]--;
    //当自减到0时，即识别为有效触摸
    if(press_counter[PAD_X]==0)
    {
      //有效触摸标志置位
      valid_touch |= (1<<PAD_X);
      //恢复有效计数器初值
      press_counter[PAD_X] = PRESS_CNT;
    }
  }
  else
  {
    //否则恢复有效计数器初值
    press_counter[PAD_X] = PRESS_CNT;
  }
  
  //方法同上
  if(LPLD_TSI_IsChxTouched(8))
  {
    press_counter[PAD_O]--;
    if(press_counter[PAD_O]==0)
    {
      valid_touch |= (1<<PAD_O);
      press_counter[PAD_O] = PRESS_CNT;
    }
  }
  else
  {
    press_counter[PAD_O] = PRESS_CNT;
  }
  
  //如果X按键为有效触摸，则打印状态
  if(valid_touch & (1<<PAD_X))
  {
    printf("X Pressed!\r\n");
  }
  
  //如果O按键为有效触摸，则打印状态
  if(valid_touch & (1<<PAD_O))
  {
    printf("\tO Pressed!\r\n");
  }
}