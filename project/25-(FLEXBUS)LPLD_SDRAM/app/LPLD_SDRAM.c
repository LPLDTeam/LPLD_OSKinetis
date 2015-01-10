/**
* --------------基于"拉普兰德K60底层库V3"的工程（LPLD_SDRAM）-----------------
* @file LPLD_SDRAM.c
* @version 0.1
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
//包含LCD设备驱动
#include "DEV_LCD.h"
//包含SDRAM设备驱动
#include "DEV_SDRAM.h"

/****************************************
说明：
*将2.8" TFT液晶屏插到RUSH开发板J1排针上。
*运行例程观察液晶屏显示结果。
*由于开发板上的SDRAM由K60的FB1控制，因此
必须先初始化控制LCD的FB0，FB1才能工作。
****************************************/

void main (void)
{
  uint8 *buff;
  
  //初始化LCD
  //包含：
  //  初始化FlexBus：FB0模块、128K寻址空间、16位宽读写等
  //  初始化ILI9320 LCD控制器
  LPLD_LCD_Init();
  
  //初始化SDRAM
  //包含：
  //  初始化FlexBus：FB1模块、256K寻址空间、8位宽读写等
  LPLD_SDRAM_Init();
  
  //动态分配SDRAM中的一段空间
  buff = LPLD_SDRAM_Malloc(10);
  
  //打印刚才分配的SDRAM空间首地址
  printf("buff's ADDR is 0x%X\r\n", buff);
  
  //将数据写入动态分配的SDRAM空间
  printf("Write buff with:\r\n\t", buff);
  for(uint8 i=0; i<10; i++)
  {
    buff[i] = i;
    printf("buff[%d]=%d, ", i, i);
  }

  //读取写入到SDRAM中的数据
  printf("\r\nRead buff:\r\n\t", buff);
  for(uint8 i=0; i<10; i++)
  {
    printf("buff[%d]=%d, ", i, buff[i]);
  }
  
  while(1)
  {
    
  }
}
