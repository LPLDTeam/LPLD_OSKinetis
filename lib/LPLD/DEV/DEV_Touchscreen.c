/**
 * @file DEV_Touchscreen.c
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
#include "DEV_Touchscreen.h"

static void LPLD_Touchscreen_Delay(int i);

/*
 * LPLD_Touchscreen_Init
 * 配置与ADS7843芯片连接的SPI模块
 * 
 * 参数:
 *    无
 * 输出：
 *    无
 */
void LPLD_Touchscreen_Init (void)
{
  SPI_InitTypeDef touch_spi_param;
 
  touch_spi_param.SPI_SPIx = TOUCHSCREEN_SPIX;
  touch_spi_param.SPI_SckDivider = SPI_SCK_DIV_32;//1.56Mhz
  touch_spi_param.SPI_Pcs0Pin = TOUCHSCREEN_PCS0;
  touch_spi_param.SPI_SckPin  = TOUCHSCREEN_SCK;
  touch_spi_param.SPI_MosiPin = TOUCHSCREEN_MOSI;
  touch_spi_param.SPI_MisoPin = TOUCHSCREEN_MISO;
  
  touch_spi_param.SPI_TxCompleteIntIsr = NULL;
  touch_spi_param.SPI_QueueEndIntIsr = NULL;
  touch_spi_param.SPI_TxFIFO_FillIntIsr = NULL;
  touch_spi_param.SPI_TxFIFO_UnderflowIntIsr = NULL;
  touch_spi_param.SPI_RxFIFO_DrainIntIsr = NULL;
  touch_spi_param.SPI_RxFIFO_OverflowIntIsr = NULL;
  
  LPLD_SPI_Init(touch_spi_param);

}

/*
 * LPLD_Touch_SendCmd
 * 向ADS7843芯片发送控制指令
 * 
 * 参数:
 *    CMD--控制命令，在DEV_Touchscreen.h中有定义
 *
 *    pcs_state--片选PCSx发送完一帧数据之后的状态
 *      |__0  保持低电平
 *      |__1  由低电平拉高到高电平     
 */
void LPLD_Touchscreen_SendCmd(uint8_t CMD,uint8_t pcs_state)
{
  LPLD_SPI_Master_WriteRead(TOUCHSCREEN_SPIX,CMD,SPI_PCS0,pcs_state);
}

/*
 * LPLD_Touchscreen_GetCovValue
 * 从ADS7843芯片获得数据
 * 
 * 参数:
 *    CMD--控制命令，在Touch.h中有定义
 *
 *    pcs_state--片选PCSx发送完一帧数据之后的状态
 *      |__0  保持低电平
 *      |__1  由低电平拉高到高电平     
 */
uint8_t LPLD_Touchscreen_GetCovValue(uint8_t CMD,uint8_t pcs_state)
{
  return LPLD_SPI_Master_WriteRead(TOUCHSCREEN_SPIX,CMD,SPI_PCS0,pcs_state);
}

/*
 * LPLD_Touchscreen_GetResult
 * 获得触摸屏位置的12位AD转换结果
 * 
 * 参数:
 *    CMD--控制命令，在Touch.h中有定义
 *
 *    pcs_state--片选PCSx发送完一帧数据之后的状态
 *      |__0  保持低电平
 *      |__1  由低电平拉高到高电平     
 */
uint16_t LPLD_Touchscreen_GetResult(uint8_t Cmd)
{
  uint16_t HI,LO,Data;
  //发送完毕CMD，CS保持在低电平状态
  LPLD_Touchscreen_SendCmd(Cmd,SPI_PCS_ASSERTED);
  //如有必要判断忙
  //在此添加判断忙函数或者延时函数，延时大于125khz即可
  LPLD_Touchscreen_Delay(500);
  //获得12位精度的高8位
  HI=LPLD_Touchscreen_GetCovValue(LPLD_CMD_DUMMY,SPI_PCS_ASSERTED);
  //获得12位精度的低4位，并将CS拉高
  LO=LPLD_Touchscreen_GetCovValue(LPLD_CMD_DUMMY,SPI_PCS_INACTIVE);
  //合成12位的转换结果
  //参考ADS7843技术文档
  Data = ((HI<<8)&0xFF00)|LO;
  Data = Data>>4; 
  return Data;
}

/*
 * LPLD_Touchscreen_Delay
 * Touchscreen延时函数
 * 
 * 参数:
 *    i--延时
 *    
 */
static void LPLD_Touchscreen_Delay(int i)
{
  int k;
  for(k=0;k<i;k++)
    asm("nop");
}
/********************************************************************/