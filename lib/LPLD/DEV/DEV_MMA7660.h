/**
 * @file DEV_MMA7660.h
 * @version 0.1[By LPLD]
 * @date 2013-09-24
 * @brief MMA7660三轴加速度传感器设备驱动程序
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
#ifndef __DEV_MMA7660_H__
#define __DEV_MMA7660_H__

#include "common.h"
#include "HW_I2C.h"

/********用户可修改值 开始***********/
//连接MMA7660用到的I2C通道
#define MMA7660_I2CX    (I2C0)

//连接MMA7660用到的SCL引脚
#define MMA7660_SCLPIN  (PTD8)

//连接MMA7660用到的SDA引脚
#define MMA7660_SDAPIN  (PTD9)
/********用户可修改值 结束***********/

//==========MMA7660 寄存器地址==================//
#define MMA7660_XOUT  0x00   // 6-bit output value X 
#define MMA7660_YOUT  0x01   // 6-bit output value Y 
#define MMA7660_ZOUT  0x02   // 6-bit output value Z
#define MMA7660_TILT  0x03   // Tilt Status 
#define MMA7660_SRST  0x04   // Sampling Rate Status
#define MMA7660_SPCNT 0x05   // Sleep Count
#define MMA7660_INTSU 0x06   // Interrupt Setup
#define MMA7660_MODE  0x07   // Mode
#define MMA7660_SR    0x08   // Auto-Wake/Sleep and 
                      // Portrait/Landscape samples 
                      // per seconds and Debounce Filter
#define MMA7660_PDET  0x09   // Tap Detection
#define MMA7660_PD    0x0A   // Tap Debounce Count
//=========MMA7660 功能参数==================//
#define MMA7660_DEV_ADDR   0x4C //Normally,can range 0x08 to 0xEF
#define MMA7660_DEV_WRITE  MMA7660_DEV_ADDR<<1 | 0x00
#define MMA7660_DEV_READ   MMA7660_DEV_ADDR<<1 | 0x01

//定义SCL Bus Speed取值，外设总线为50Mhz时的计算结果
#define MMA7660_SCL_50KHZ                   (0x33) 
#define MMA7660_SCL_100KHZ                  (0x2B)  
#define MMA7660_SCL_150KHZ                  (0x28)
#define MMA7660_SCL_200KHZ                  (0x23)
#define MMA7660_SCL_250KHZ                  (0x21)
#define MMA7660_SCL_300KHZ                  (0x20)
#define MMA7660_SCL_400KHZ                  (0x17)  

//函数声明
void LPLD_MMA7660_Init(void);
void LPLD_MMA7660_WriteReg(uint8, uint8);
uint8 LPLD_MMA7660_ReadReg(uint8);
int8 LPLD_MMA7660_GetResult(uint8);

#endif 