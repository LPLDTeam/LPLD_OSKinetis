/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_VirtualSerialComm）-----------------
 * @file LPLD_VirtualSerialComm.c
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

/********************************************
 * 说明: 
 * 将MiniUSB线插入RUSH Kinetis开发板Card上的USB
   插座，并连接至电脑USB接口。
 * 安装lib\usb\LPLD_CDC_class_Driver_Kinetis CDC驱动
 * 使用串口调试助手波特率任意设置。
 * 使用串口调试助手发送数据,单片机接收到数据后，将数据返回。
 ********************************************/
#include "common.h"
#include "HW_USB.h"
uint8_t rx_len = 0;
uint8_t rx_buf[64];

void usb_rev_handle(void);
/********************************************************************/
void main (void)
{
  LPLD_USB_Init();                    //初始化K60外设
  LPLD_USB_SetRevIsr(usb_rev_handle); //设置USB接收中断
  while(1)
  {
  
  } 
}

/*
*  usb_rev_handle
*  USB中断接收函数
*/
void usb_rev_handle(void)
{
  rx_len = LPLD_USB_VirtualCom_Rx(rx_buf);
  LPLD_USB_QueueData();
  LPLD_USB_VirtualCom_Tx(rx_buf,rx_len);
}

