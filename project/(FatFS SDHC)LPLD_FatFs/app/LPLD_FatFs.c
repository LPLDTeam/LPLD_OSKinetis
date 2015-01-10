/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_FatFs）-----------------
 * @file LPLD_FatFs.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用SDHC+FatFs文件系统，对SD卡上的文件进行读写。
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
   *如果运行错误，建议在PC上先格式化SD卡再
    插入到核心板上。或者更换SDHC大容量卡。
 ****************************************/

// 打印文件返回代码
void die(FRESULT rc)
{
  printf("错误代码 rc=%u.\n", rc);
  for (;;) ;
}
// 用户自定义的为FatFs系统提供实时时间的函数
DWORD get_fattime (void)
{
  return ((DWORD)(2013 - 1980) << 25)	//2013年
       | ((DWORD)3 << 21)               //3月
       | ((DWORD)15 << 16)              //15日
       | ((DWORD)0 << 11)
       | ((DWORD)0 << 5)
       | ((DWORD)0 >> 1);
}

/********************************************************************/
void main (void)
{
  uint16 i;
  // 以下变量定义均采用FatFs文件系统变量类型
  FRESULT rc;			//结果代码 
  FATFS fatfs;			// 文件系统对象 
  FIL fil;			// 文件对象 
  UINT bw, br;
  BYTE buff[128];
  
  // 注册一个磁盘工作区 
  f_mount(0, &fatfs);		
  //创建一个新的txt文档
  printf("新建一个文件 (LPLD_FatFs.TXT).\n");
  rc = f_open(&fil, "0:/LPLD_FatFs.TXT", FA_WRITE | FA_CREATE_ALWAYS);
  if (rc) die(rc);
  
  //向新创建的文档中写入"Hello LPLD!"并换行
  printf("写入文本数据. (Hello LPLD!)\n");
  rc = f_write(&fil, "Hello LPLD!\r\n", 13, &bw);
  if (rc) die(rc);
  printf("共写入 %u Bytes.\n", bw);
  
  //关闭新建的文件
  printf("关闭文件.\n\n");
  rc = f_close(&fil);
  if (rc) die(rc);
  
  //打开刚才新建的文件
  printf("打开一个文件 (LPLD_FatFs.TXT).\n");
  rc = f_open(&fil, "0:/LPLD_FatFs.TXT", FA_READ);
  if (rc) die(rc);
  
  //打印出文件内的内容
  printf("打印此文件内容.\n");
  for (;;) 
  {
    rc = f_read(&fil, buff, sizeof(buff), &br);	// 读取文件的一块 
    if (rc || !br) break;			// 错误或读取完毕 
    for (i = 0; i < br; i++)		        // 输出读取的字节数据 
      LPLD_UART_PutChar(TERM_PORT, buff[i]);
  }
  if (rc) die(rc);
  
  //关闭文件
  printf("\n关闭文件.\n");
  rc = f_close(&fil);
  if (rc) die(rc);
  
  printf("文件系统测试完毕.\n");

  for (;;) 
  {
  }
  
}
