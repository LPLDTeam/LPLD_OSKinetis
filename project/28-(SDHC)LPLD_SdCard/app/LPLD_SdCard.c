/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_SdCard）-----------------
 * @file LPLD_SdCard.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用SDHC模块，实现SD卡的数据读写。
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
#include "common.h"

/****************************************
 说明：
   *将MicroSD卡插入到K60核心板的SD卡插槽。
   *将MiniUSB线插入RUSH Kinetis开发板的USB
    插座，并连接至电脑USB接口。
   *使用串口调试助手波特率设置为115200
   *使用串口调试助手查看运行结果
 ****************************************/
//进行读写操作的块数
#define BLOCK_LEN     2
#define BUFFER_SIZE   512*BLOCK_LEN

/********************************************************************/
void main (void)
{
  uint32 i;	
  //读写数据数组
  uint8  sdhc_dat_buffer1[BUFFER_SIZE];    
  //SD卡操作返回值
  SDHCRES sd_state;    
  
  //关中断
  DisableInterrupts;                                      
  
  //SDHC初始化
  sd_state = (SDHCRES)LPLD_SDHC_InitCard();  
  
  //开中断
  EnableInterrupts;			                            
  
  //判断SD卡操作状态
  if (SDHCRES_OK == sd_state)
  {
    printf("\r\nSD init success.\r\n");
    //读(多)扇区操作，并返回SD操作状态
    sd_state = LPLD_SDHC_ReadBlocks(sdhc_dat_buffer1, 0, BLOCK_LEN);
  }
  else
  {
    printf("\r\nSD init failure.\r\n");
  }
  
  //判断SD卡操作状态
  if (SDHCRES_OK == sd_state)
  {
    printf("Read disk success.\r\n");
    printf("SD data: \r\n");
    //若读成功，打印该扇区内容
    for(i = 0;i < BUFFER_SIZE;i++) 
    {
      printf("%02X ", sdhc_dat_buffer1[i]);
    }  
    printf("\r\n");
  }
  else
  {
    printf("Read disk failed.\n\r");
  }
  
  
  //判断SD卡操作状态
  if (SDHCRES_OK == sd_state)
  {
    printf("Update data success.\r\n");
    //更新数据数组，自加1
    for (i = 0; i < BUFFER_SIZE; i++)
    {
      sdhc_dat_buffer1[i] ++;
    }
    //写(多)扇区操作，并返回SD操作状态
    sd_state = LPLD_SDHC_WriteBlocks(sdhc_dat_buffer1, 0, BLOCK_LEN);  
  }
  
  //判断SD卡操作状态
  if (SDHCRES_OK == sd_state)
  {
    printf("Write Disk success.\r\n");
    //再次读(多)扇区操作，并返回SD操作状态
    sd_state = LPLD_SDHC_ReadBlocks(sdhc_dat_buffer1, 0, BLOCK_LEN);
  }
  else
  {
    printf("Write Disk failure.\r\n");
  }
  
  //判断SD卡操作状态
  if (SDHCRES_OK == sd_state)
  {
    printf("Read disk success.\r\n");
    printf("SD data: \r\n");
    //若读成功，打印该扇区内容
    for(i = 0;i < BUFFER_SIZE;i++) 
    {
      printf("%02X ", sdhc_dat_buffer1[i]);
    }
    printf("\r\n");
  }
  else
  {
    printf("Read disk failure.\r\n");
  }
  
  while(1)
  {
    
  } 
}
