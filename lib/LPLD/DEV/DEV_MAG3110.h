/**
 * @file DEV_MAG3110.h
 * @version 0.2[By LPLD]
 * @date 2013-10-3
 * @brief MAG3110三轴地磁传感器设备驱动程序
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
#ifndef __DEV_MAG3110_H__
#define __DEV_MAG3110_H__

#include "common.h"
#include "HW_I2C.h"

/********用户可修改值 开始***********/
//连接MAG3110用到的I2C通道
#define MAG3110_I2CX    (I2C0)

//连接MAG3110用到的SCL引脚
#define MAG3110_SCLPIN  (PTB0)

//连接MAG3110用到的SDA引脚
#define MAG3110_SDAPIN  (PTB1)
/********用户可修改值 结束***********/

//==========MAG3110 寄存器地址==================//
#define MAG3110_DR_STATUS  0x00

#define MAG3110_OUT_X_MSB  0x01
#define MAG3110_OUT_X_LSB  0x02

#define MAG3110_OUT_Y_MSB  0x03
#define MAG3110_OUT_Y_LSB  0x04

#define MAG3110_OUT_Z_MSB  0x05
#define MAG3110_OUT_Z_LSB  0x06

#define MAG3110_WHO_AM_I   0x07
#define MAG3110_SYSMOD     0x08

#define MAG3110_OFF_X_MSB  0x09 
#define MAG3110_OFF_X_LSB  0x0A 

#define MAG3110_OFF_Y_MSB  0x0B 
#define MAG3110_OFF_Y_LSB  0x0C

#define MAG3110_OFF_Z_MSB  0x0D 
#define MAG3110_OFF_Z_LSB  0x0E

#define MAG3110_DIE_TEMP   0x0F

#define MAG3110_CTRL_REG1  0x10
#define MAG3110_CTRL_REG2  0x11

//=========MAG3110 功能参数==================//
#define MAG3110_DEV_ADDR   0x0E //Normally,can range 0x08 to 0xEF
#define MAG3110_DEV_WRITE  MAG3110_DEV_ADDR<<1 | 0x00
#define MAG3110_DEV_READ   MAG3110_DEV_ADDR<<1 | 0x01
#define MAG3110_DEV_ID     0xC4
   
#define MAG3110_80HZ_OSR1_ACTIVE_MODE  0x01
#define MAG3110_AUTO_MRST_DISABLE      0x00

//=========MAG3110 DR_STATUS寄存器 寄存器位==================//
#define DR_STATUS_X_READY   0x01
#define DR_STATUS_Y_READY   0x02
#define DR_STATUS_Z_READY   0x04
#define DR_STATUS_ALL_READY 0x08
#define DR_STATUS_X_OW      0x10
#define DR_STATUS_Y_OW      0x20
#define DR_STATUS_Z_OW      0x40
#define DR_STATUS_ALL_OW    0x80

//=========MAG3110 SYSMOD寄存器 寄存器位==================//                               
#define MAG3110_STANDBY_MODE           0x00   //
#define MAG3110_ACTIVE_RAWDATA_MODE    0x01   //
#define MAG3110_ACTIVE_NO_RAWDATA_MODE 0x02   //

//=========MAG3110 CTRL_REG1寄存器 寄存器位===============//
#define CTRL_REG1_DR_MARK  0x70
#define CTRL_REG1_DR_SHIFT 5
#define CTRL_REG1_DR(x)    (((uint8_t)(((uint8_t)(x))<<CTRL_REG1_DR_SHIFT))& CTRL_REG1_DR_MARK)
#define CTRL_REG1_OS_MARK  0x18
#define CTRL_REG1_OS_SHIFT 3
#define CTRL_REG1_OS(x)    (((uint8_t)(((uint8_t)(x))<<CTRL_REG1_OS_SHIFT))& CTRL_REG1_OS_MARK)
#define CTRL_REG1_FR_MARK  0x04
#define CTRL_REG1_FR_SHIFT 2
#define CTRL_REG1_TM_MARK  0x02
#define CTRL_REG1_TM_SHIFT 1
#define CTRL_REG1_AC_MARK  0x01
#define CTRL_REG1_AC_SHIFT 0

//=========MAG3110 CTRL_REG2寄存器 寄存器位===============//
#define CTRL_REG2_AUTO_MRST_EN_MARK  0x80
#define CTRL_REG2_AUTO_MRST_EN_SHIFT 7
#define CTRL_REG2_RAW_MARK           0x20
#define CTRL_REG2_RAW_SHIFT          5
#define CTRL_REG2_Mag_RST_MARK       0x10
#define CTRL_REG2_Mag_RST_SHIFT      4

//定义SCL Bus Speed取值，外设总线为50Mhz时的计算结果
#define MAG3110_SCL_50KHZ                   (0x33) 
#define MAG3110_SCL_100KHZ                  (0x2B)  
#define MAG3110_SCL_150KHZ                  (0x28)
#define MAG3110_SCL_200KHZ                  (0x23)
#define MAG3110_SCL_250KHZ                  (0x21)
#define MAG3110_SCL_300KHZ                  (0x20)
#define MAG3110_SCL_400KHZ                  (0x17)  

//函数声明
uint8 LPLD_MAG3110_Init(void);
void LPLD_MAG3110_WriteReg(uint8, uint8);
uint8 LPLD_MAG3110_ReadReg(uint8);
int16 LPLD_MAG3110_GetResult(uint8, uint8);

#endif 