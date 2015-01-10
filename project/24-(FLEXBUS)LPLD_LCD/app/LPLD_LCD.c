/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_LCD）-----------------
 * @file LPLD_LCD.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用FlexBus模块，控制外部LCD液晶屏的显示。
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
//包含LCD设备驱动
#include "DEV_LCD.h"

/****************************************
 说明：
   *将2.8" TFT液晶屏插到RUSH开发板J1排针上。
   *运行例程观察液晶屏显示结果。
 ****************************************/

void main (void)
{
  //初始化LCD
  //包含：
  //  初始化FlexBus：FB0模块、128K寻址空间、16位宽读写等
  //  初始化ILI9320 LCD控制器
  LPLD_LCD_Init();
  
  //初始化LCD后延时一段时间
  LPLD_LPTMR_DelayMs(10);
  
  //填充LCD背景色：黑色
  LPLD_LCD_FillBackground(COLOR_Black);
  
  //打印一行字符串：白色字体、黑色背景
  LPLD_LCD_PrintStringLine("Hello World! ", COLOR_White, COLOR_Black);
  
  //接上一行打印字符串：红色字体、黑色背景
  LPLD_LCD_PrintString("RUSH Kinetis!", COLOR_Red, COLOR_Black);
  
  //打印一行数字：白色字体、黑色背景
  LPLD_LCD_PrintNumLine(9320, 4, COLOR_White, COLOR_Black);
  
  //在指定坐标开始显示一条横线
  //x,y起始坐标(0,50) x,y结束坐标(320,50)
  LPLD_LCD_DrawLine(0, 50, 320, 50, COLOR_Blue); 
  
  //在指定坐标填充一个矩形
  //x,y起始坐标(80,80) x长度150 y长度100
  LPLD_LCD_DrawRectangle(80, 80, 150, 100, COLOR_Magenta, COLOR_Yellow);
  
  while(1)
  {

  } 
}

