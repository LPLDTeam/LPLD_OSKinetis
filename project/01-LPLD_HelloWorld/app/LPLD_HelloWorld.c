/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_HelloWorld）-----------------
 * @file LPLD_HelloWorld.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 让Kinetis运行起来，通过串口输出调试信息。
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
   *使用串口调试助手查看运行结果
 ****************************************/
GPIO_InitTypeDef gpio_init;

void main (void)
{
  char ch;
  printf("Hello World!!\r\n");
  while(1)
  {
    ch = in_char();
    out_char(ch);
  } 
}


