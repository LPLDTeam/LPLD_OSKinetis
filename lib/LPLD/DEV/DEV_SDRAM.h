/**
 * @file DEV_SDRAM.h
 * @version 0.1[By LPLD]
 * @date 2013-09-24
 * @brief SDRAM设备驱动程序
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
#ifndef __DEV_SDRAM_H__
#define __DEV_SDRAM_H__

#include "common.h"

/********用户可修改值 开始***********/
//SDRAM总线基地址
#define SDRAM_ADDRESS   (0x70000000)
//SDRAM寻址空间大小
//RUSH开发板采用的SDRAM型号为IS61LV12816，容量为2M bit
//空间大小按字节换算为256KB
#define SDRAM_SIZE      (256) 
 
//初始化SDRAM所需的FlexBus
void LPLD_SDRAM_Init(void);
//在SDRAM中动态分配一段空间
void *LPLD_SDRAM_Malloc(unsigned);
//释放在SDRAM中动态分配的空间
void LPLD_SDRAM_Free(void *);

#endif 
