/**
 * @file DEV_Touchscreen.h
 * @version 1.0[By LPLD]
 * @date 2013-09-24
 * @brief Touchscreen设备驱动程序
 *
 * 更改建议:可根据实际硬件修改
 *
 * 版权所有:北京拉普兰德电子技术有限公司
 * http://www.lpld.cn
 * mail:support@lpld.cn
 *
 * @par
 * 本代码由拉普兰德[LPLD]开发并维护，并向所有使用者开放源代码。
 * 开发者可以随意修使用或改源代码。但本段及以上注释应予以保留。
 * 不得更改或删除原版权所有者姓名，二次开发者可以加注二次版权所有者。
 * 但应在遵守此协议的基础上，开放源代码、不得出售代码本身。
 * 拉普兰德不负责由于使用本代码所带来的任何事故、法律责任或相关不良影响。
 * 拉普兰德无义务解释、说明本代码的具体原理、功能、实现方法。
 * 除非拉普兰德[LPLD]授权，开发者不得将本代码用于商业产品。
 */

#ifndef __DEV_TOUCHSCREEN_H__
#define __DEV_TOUCHSCREEN_H__

#include "common.h"
#include "HW_SPI.h"

/********用户可修改值 开始**********/
//选择TOUCHSCREEN使用的SPI模块
#define TOUCHSCREEN_SPIX    (SPI2)

//选择TOUCHSCREEN MOSI所对应的IO
#define TOUCHSCREEN_MOSI  (PTD13)

//选择TOUCHSCREEN MISO所对应的IO
#define TOUCHSCREEN_MISO  (PTD14)

//选择TOUCHSCREEN SCK所对应的IO
#define TOUCHSCREEN_SCK  (PTD12)

//选择TOUCHSCREEN PCS0所对应的IO
#define TOUCHSCREEN_PCS0  (PTD11)

//==========定义ADS7843发送控制指令===================
//ADS7843发送控制命令格式：
//bit7 bit6  bit5 bit4 bit3    bit2 bit1 bit0
//   S   A2    A1   A0 MODE SER/DFR  PD1  PD0

/*A2  A1  A0 SER/ DFR  AnalogInput  X Sw  Y Sw  +REF   –REF
* 0   0   1   1        X+           OFF   ON    VREF   GND  
* 0   1   0   1        IN3          OFF   OFF   VREF   GND  
* 1   0   1   1        Y+           ON    OFF   VREF   GND  
* 1   1   0   1        IN4          OFF   OFF   VREF   GND  
* 0   0   1   0        X+           OFF   ON    Y+     Y?  
* 1   0   1   0        Y+           ON    OFF   X+     X? 
* 1   1   0   0        Outputs Identity Code, 1000 0000 0000*/   

#define CHIP_ID      (0x0800)
//定义开始标志
#define INIT_START   (0x80)
//定义X或Y 开启
#define Y_SWITCH_ON  (0x10)
#define X_SWITCH_ON  (0x50)
#define ID_CODE      (0x60)
//定义转换模式
#define MODE_8BITS   (0x08)
#define MODE_12BITS  (0x00)
//定义单端/差分模式
#define SINGLEEND    (0x04)
#define DIFFERENTIAL (0x00)
//定义功耗模式和PENIRQ是否是能
#define POWERDOWN_ENABLE_PENIRQ   (0x00)//在转换间隔芯片POWERDOWN PENIRQ使能
#define POWERDOWN_DISABLED_PENIRQ (0x01)//在转换间隔芯片POWERDOWN PENIRQ禁止
#define POWERUP_ENABLE_PENIRQ     (0x02)//在转换间隔芯片保持POWERUP PENIRQ使能
#define POWERUP_DISABLED_PENIRQ   (0x03)//在转换间隔芯片保持POWERUP PENIRQ禁止

//配置成X轴采集、12位精度、单端、持续POWERUP模式、PENIRQ禁止
#define LPLD_SER_SAMPLE_X   (INIT_START|X_SWITCH_ON|MODE_12BITS|SINGLEEND|POWERDOWN_ENABLE_PENIRQ)//11010111B
//配置成Y轴采集、12位精度、单端、持续POWERUP模式、PENIRQ禁止  
#define LPLD_SER_SAMPLE_Y   (INIT_START|Y_SWITCH_ON|MODE_12BITS|SINGLEEND|POWERDOWN_ENABLE_PENIRQ)//10010111B 
//获取ID
#define LPLD_GET_CHIPID (INIT_START|ID_CODE|MODE_12BITS|DIFFERENTIAL|POWERUP_DISABLED_PENIRQ)
//
#define LPLD_CMD_DUMMY       0x00

/********用户可修改值 结束**********/

//Touchscreen初始化函数
void LPLD_Touchscreen_Init (void);
//Touchscreen获得X或Y轴位置AD转换结果函数
uint16 LPLD_Touchscreen_GetResult(uint8);
//Touchscreen发送命令函数
void LPLD_Touchscreen_SendCmd(uint8,uint8);
//Touchscreen获得转换结果函数
uint8 LPLD_Touchscreen_GetCovValue(uint8,uint8);

#endif