/**
 * @file DEV_Nrf24L01.h
 * @version 1.0[By LPLD]
 * @date 2013-09-24
 * @brief Nrf24L01设备驱动程序
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
#ifndef __DEV_NRF24L01_H__
#define __DEV_NRF24L01_H__

#include "common.h"
#include "HW_SPI.h"
#include "HW_GPIO.h"

/********用户可修改值 开始**********/
//选择NRF24L01使用的SPI模块
#define NRF24L01_SPIX    (SPI0)

//选择NRF24L01 MOSI所对应的IO
#define NRF24L01_MOSI  (PTA16)

//选择NRF24L01 MISO所对应的IO
#define NRF24L01_MISO  (PTA17)

//选择NRF24L01 SCK所对应的IO
#define NRF24L01_SCK  (PTA15)

//选择NRF24L01 PCS0所对应的IO
#define NRF24L01_PCS0  (PTA14)

//选择NRF24L01 CE所对应的IO
#define NRF24L01_CE_PTX  (PTB)
#define NRF24L01_CE_IOX  (GPIO_Pin0)
#define NRF24L01_CE_H    LPLD_GPIO_Output_b(NRF24L01_CE_PTX,NRF24L01_CE_IOX,OUTPUT_H) 
#define NRF24L01_CE_L    LPLD_GPIO_Output_b(NRF24L01_CE_PTX,NRF24L01_CE_IOX,OUTPUT_L) 

// NRF24L01相关参数定义
#define NRF24L01_PLOAD_LEN        (32) //数据载荷长度
#define NRF24L01_TX_ADR_LEN       (5)  //TX地址长度
#define NRF24L01_RX_ADR_LEN       (5)  //RX地址长度

// Nrf24L01_CheckID
#define NRF24L01_RX_ADDR_P0_RESET_ID (0xE7E7E7E7)
#define NRF24L01_RX_ADDR_P1_RESET_ID (0xC2C2C2C2)
#define NRF24L01_TX_ADDR_RESET_ID    (0xE7E7E7E7)
// NRF24L01指令
#define NRF24L01_READ_REG         0x00   // 读配置寄存器
#define NRF24L01_WRITE_REG        0x20   // 写配置寄存器
#define NRF24L01_RD_RX_PLOAD      0x61   // 读取RX FIFO中的数据
#define NRF24L01_WR_TX_PLOAD      0xA0   // 向TX FIFO中写入数据
#define NRF24L01_FLUSE_TX         0xE1   // 清除TX FIFO中的数据 应用于发射模式下
#define NRF24L01_FLUSE_RX         0xE2   // 清除RX FIFO中的数据 应用于接收模式下
#define NRF24L01_REUSE_TX_PL      0xE3   // 重新使用上一包有效数据
#define NRF24L01_NOP              0xFF   // 无操作 用于读STATUS寄存器
//********************************************************************************************************************// 
// NRF24L01寄存器(地址)
#define NRF24L01_CONFIG           0x00  //配置发送状态，CRC校验模式以及发收发状态响应方式
#define NRF24L01_EN_AA            0x01  //自动应答功能设置
#define NRF24L01_EN_RXADDR        0x02  //可用信道设置
#define NRF24L01_SETUP_AW         0x03  //收发地址宽度设置
#define NRF24L01_SETUP_RETR       0x04  //自动重发功能设设置
#define NRF24L01_RF_CH            0x05  //工作信道设定
#define NRF24L01_RF_SETUP         0x06  //发射速率、功耗功能设置
#define NRF24L01_STATUS           0x07  //状态寄存器
#define NRF24L01_OBSERVE_TX       0x08  //发送监测功能
#define NRF24L01_CD               0x09  //地址检测 
#define NRF24L01_RX_ADDR_P0       0x0A  //频道0接收数据地址
#define NRF24L01_RX_ADDR_P1       0x0B  //频道1接收数据地址
#define NRF24L01_RX_ADDR_P2       0x0C  //频道2接收数据地址
#define NRF24L01_RX_ADDR_P3       0x0D  //频道3接收数据地址
#define NRF24L01_RX_ADDR_P4       0x0E  //频道4接收数据地址
#define NRF24L01_RX_ADDR_P5       0x0F  //频道5接收数据地址
#define NRF24L01_TX_ADDR          0x10  //发送地址寄存器
#define NRF24L01_RX_PW_P0         0x11  //接收频道0接收数据长度
#define NRF24L01_RX_PW_P1         0x12  //接收频道1接收数据长度
#define NRF24L01_RX_PW_P2         0x13  //接收频道2接收数据长度
#define NRF24L01_RX_PW_P3         0x14  //接收频道3接收数据长度
#define NRF24L01_RX_PW_P4         0x15  //接收频道4接收数据长度
#define NRF24L01_RX_PW_P5         0x16  //接收频道5接收数据长度
#define NRF24L01_FIFO_STATUS      0x17  //FIFO栈入栈出状态寄存器设置
//NRF24L01状态寄存器值
#define NRF24L01_STATUS_MAX_RT    0x10 //达到最大重发状态
#define NRF24L01_STATUS_TX_DS     0x20 //发送成功
#define NRF24L01_STATUS_RX_DR     0x40 //接收成功

//NRF24L01初始化函数
uint8 LPLD_Nrf24L01_Init(void);
//NRF24L01发送模式初始化函数
uint8 LPLD_Nrf24L01_TxModeInit(void);
//NRF24L01接收模式初始化函数
uint8 LPLD_Nrf24L01_RxModeInit(void);
//NRF24L01发送数据帧函数
uint8 LPLD_Nrf24L01_SendFrame(uint8 *data, uint8 len);
//NRF24L01接收数据帧函数
void LPLD_Nrf24L01_RecvFrame(uint8 *data, uint8 len);
#endif
