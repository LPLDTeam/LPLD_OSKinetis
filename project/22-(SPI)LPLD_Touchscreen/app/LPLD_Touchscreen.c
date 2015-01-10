/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_TouchScreen）-----------------
 * @file LPLD_TouchScreen.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用SPI模块，通过AD7843控制电阻式触摸屏。
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
#include "DEV_Touchscreen.h"

/****************************************
 说明：
   *将MiniUSB线插入RUSH Kinetis开发板的USB
    插座，并连接至电脑USB接口。
   *使用串口调试助手波特率设置为115200
   *使用串口调试助手查看运行结果
   *将2.8" TFT液晶屏插到RUSH开发板J1排针上。
   *点击屏幕不同位置观察运行结果。
 ****************************************/

uint16 X_Positon = 0; //声明X轴变量
uint16 Y_Positon = 0; //声明Y轴变量

void main (void)
{
  LPLD_Touchscreen_Init(); //初始化SPI
  
  while(1)
  {
    X_Positon=LPLD_Touchscreen_GetResult(LPLD_SER_SAMPLE_X);
    Y_Positon=LPLD_Touchscreen_GetResult(LPLD_SER_SAMPLE_Y);
    printf("X,Y:\t%d\t%d\r\n",X_Positon, Y_Positon);
    LPLD_LPTMR_DelayMs(100);
  } 
}

/********************************************************************/
