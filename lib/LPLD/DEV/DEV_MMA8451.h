/**
 * @file DEV_MMA8451.h
 * @version 0.1[By LPLD]
 * @date 2013-09-24
 * @brief MMA8451三轴加速度传感器设备驱动程序
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
#ifndef __DEV_MMA8451_H__
#define __DEV_MMA8451_H__

#include "common.h"
#include "HW_I2C.h"

/********用户可修改值 开始***********/
//连接MMA8451用到的I2C通道
#define MMA8451_I2CX    (I2C0)

//连接MMA8451用到的SCL引脚
#define MMA8451_SCLPIN  (PTB0)

//连接MMA8451用到的SDA引脚
#define MMA8451_SDAPIN  (PTB1)
/********用户可修改值 结束***********/

//==========MMA8451 寄存器地址==================//
#define MMA8451_REG_STATUS         0x00    //状态寄存器
#define MMA8451_REG_OUTX_MSB       0x01    //14位采样结果高8位[7:0]
#define MMA8451_REG_OUTX_LSB       0x02    //14位采样结果低6位[7:2]
#define MMA8451_REG_OUTY_MSB       0x03    //14位采样结果高8位[7:0]
#define MMA8451_REG_OUTY_LSB       0x04    //14位采样结果低6位[7:2]
#define MMA8451_REG_OUTZ_MSB       0x05    //14位采样结果高8位[7:0]
#define MMA8451_REG_OUTZ_LSB       0x06    //14位采样结果低6位[7:2]
#define MMA8451_REG_SYSMOD         0x0B    //当前系统模式
#define MMA8451_REG_WHOAMI         0x0D    //设备ID寄存器
#define MMA8451_REG_CTRL_REG1      0x2A    //系统控制寄存器1
#define MMA8451_REG_CTRL_REG2      0x2B    //系统控制寄存器2

//=========MMA8451 功能参数==================//
#define MMA8451_DEV_ADDR   0x1D //Normally,can range 0x08 to 0xEF
#define MMA8451_DEV_WRITE  MMA8451_DEV_ADDR<<1 | 0x00
#define MMA8451_DEV_READ   MMA8451_DEV_ADDR<<1 | 0x01
#define MMA8451_DEV_ID     0x1A   //MMA8451设备ID

//=========MMA8451 STATUS寄存器 寄存器位==================//
#define MMA8451_STATUS_X_READY   0x01
#define MMA8451_STATUS_Y_READY   0x02
#define MMA8451_STATUS_Z_READY   0x04
#define MMA8451_STATUS_XYZ_READY 0x08
#define MMA8451_STATUS_X_OW      0x10
#define MMA8451_STATUS_Y_OW      0x20
#define MMA8451_STATUS_Z_OW      0x40
#define MMA8451_STATUS_XYZ_OW    0x80

//定义SCL Bus Speed取值，外设总线为50Mhz时的计算结果
#define MMA8451_SCL_50KHZ                   (0x33) 
#define MMA8451_SCL_100KHZ                  (0x2B)  
#define MMA8451_SCL_150KHZ                  (0x28)
#define MMA8451_SCL_200KHZ                  (0x23)
#define MMA8451_SCL_250KHZ                  (0x21)
#define MMA8451_SCL_300KHZ                  (0x20)
#define MMA8451_SCL_400KHZ                  (0x17)  

//函数声明
uint8 LPLD_MMA8451_Init(void);
void LPLD_MMA8451_WriteReg(uint8, uint8);
uint8 LPLD_MMA8451_ReadReg(uint8);
int16 LPLD_MMA8451_GetResult(uint8, uint8);

#endif 