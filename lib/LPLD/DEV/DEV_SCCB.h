/**
 * @file DEV_SCCB.h
 * @version 0.1[By LPLD]
 * @date 2013-09-24
 * @brief SCCB设备驱动程序
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
#ifndef __DEV_SCCB_H__
#define __DEV_SCCB_H__

#include "common.h"
   
#define ADR_OV7670      0x42

/********用户可修改值 开始***********/
//定义SCCB设备地址
#define SCCB_DEV_ADR    ADR_OV7670
//定义SCL、SDA的引脚
#define SCCB_SCL        PTB0_O  
#define SCCB_SDA_O      PTB1_O 
#define SCCB_SDA_I      PTB1_I 
//定义SDA输入输出
#define SCCB_SDA_OUT()  DDRB1=1
#define SCCB_SDA_IN()   DDRB1=0
/********用户可修改值 结束***********/

#define SCCB_DELAY()	LPLD_SCCB_Delay(5000)	

void LPLD_SCCB_Init(void);
uint8 LPLD_SCCB_WriteReg(uint16, uint8);
uint8 LPLD_SCCB_ReadReg(uint8, uint8*, uint16);

#endif 
