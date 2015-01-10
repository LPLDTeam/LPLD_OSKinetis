/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_VirtualMouse）-----------------
 * @file LPLD_VirtualMouse.c
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

/*****************************************************
 * 说明: 
 * 使用HID Mouse时需要在K60_Card中
   先将USB_DEVICE_CLASS定义为 USB_DEVICE_CLASS_HID
 * 将MiniUSB线插入RUSH Kinetis开发板Card上的USB
   插座，并连接至电脑USB接口。
 * 按下开发板上的S1和S2按键控制鼠标移动。
 ****************************************************/
#include "common.h"
#include "HW_USB.h" 
#include "HW_GPIO.h"
#include "mouse_button.h"
   
GPIO_InitTypeDef Key_left;
GPIO_InitTypeDef Key_right;
   
uint8 Mouse[4] = {0};

void key_isr(void);

void main (void)
{
  Key_left.GPIO_PTx  = PTB;
  Key_left.GPIO_Pins = GPIO_Pin6;
  Key_left.GPIO_PinControl = INPUT_PULL_UP | IRQC_FA;
  Key_left.GPIO_Dir  = DIR_INPUT;
  Key_left.GPIO_Isr  = key_isr;
  
  Key_right.GPIO_PTx  = PTB;
  Key_right.GPIO_Pins = GPIO_Pin7;
  Key_right.GPIO_PinControl = INPUT_PULL_UP | IRQC_FA;
  Key_right.GPIO_Dir  = DIR_INPUT;
  Key_right.GPIO_Isr  = key_isr;
  
  LPLD_GPIO_Init(Key_left); 
  LPLD_GPIO_Init(Key_right); 
  LPLD_GPIO_EnableIrq(Key_left);
  
  LPLD_USB_Init();
  
  while(1)
  {
    LPLD_USB_HID_LoopTask(Mouse);   
  } 
}

void key_isr(void)
{       
  if(LPLD_GPIO_IsPinxExt(PORTB,GPIO_Pin6))
  {
    Mouse[MOUSE_BUTTON_CLICK] = MOUSE_LEFT_CLICK; //鼠标左键
    //Mouse[MOUSE_X_MOVEMENT] = MOUSE_LEFT_MOVE;  //鼠标左移
    //Mouse[MOUSE_Y_MOVEMENT] = MOUSE_UP_MOVE;    //鼠标上移
    
  }
  else if(LPLD_GPIO_IsPinxExt(PORTB,GPIO_Pin7))
  {
    Mouse[MOUSE_BUTTON_CLICK] = MOUSE_RIGHT_CLICK;//鼠标右键
    //Mouse[MOUSE_X_MOVEMENT] = MOUSE_RIGHT_MOVE; //鼠标右移
    //Mouse[MOUSE_Y_MOVEMENT] = MOUSE_DOWN_MOVE;  //鼠标下移
  }
}

