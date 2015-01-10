/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_Flash）-----------------
 * @file LPLD_Flash.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用Flash模块，实现片内Flash的数据读写。
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
   *将MiniUSB线插入RUSH Kinetis开发板的USB
    插座，并连接至电脑USB接口。
   *使用串口调试助手波特率设置为115200
   *使用串口调试助手查看运行结果
 ****************************************/

#define FLASH_TEST_SECTOR   (127)
#define FLASH_TEST_ADDR     (FLASH_TEST_SECTOR*2048)// FLASH_TEST_SECTOR*(2048)
#define DATA_SIZE           (10)

//变量定义
uint8 read_buffer[DATA_SIZE];  //定义读缓冲区
uint8 write_buffer[DATA_SIZE]; //定义写缓冲区

void delay(void);

void main (void)
{
  uint8 read;
  uint8 *ptr;
  uint8 result=NULL;
  int len;
  
  LPLD_Flash_Init();
  
  printf("-------FLASH擦除写入测试例程-------\r\n");
  printf(" E：擦除%d号扇区内容。\r\n", FLASH_TEST_SECTOR);
  printf(" R：读取%d号扇区内容(10字节)。\r\n", FLASH_TEST_SECTOR);
  printf(" W：写入%d号扇区（最大10字节）。\r\n", FLASH_TEST_SECTOR);
  
  while(1)
  { 
    read = LPLD_UART_GetChar(UART5);
    
    switch(read)
    {
      //先执行擦出操作
      case 'E':
      case 'e':  
        
        DisableInterrupts;  
        result=LPLD_Flash_SectorErase(FLASH_TEST_ADDR);
        EnableInterrupts;
        delay();
        
        if(result==FLASH_OK)
        {
          printf("擦除成功。\r\n");
        }
        else
        {
          printf("擦除失败，错误代码:%X。\r\n", result);
        }
        break;
        
      //再执行写入操作 
      case 'W':
      case 'w':
        
        len=0;
        
        printf("请输入10个字符：\r\n");
        
        while(len < DATA_SIZE)
        {
          write_buffer[len++] = LPLD_UART_GetChar(UART5); 
        }
      
        DisableInterrupts;  
        result=LPLD_Flash_ByteProgram((uint32)FLASH_TEST_ADDR, (uint32*)write_buffer, DATA_SIZE);
        EnableInterrupts;
        delay();
        
        if(result==FLASH_OK)
        {
          printf("写入成功。\r\n");
        }
        else
        {
          printf("写入失败，错误代码:%X。\r\n", result);
        }
        break;
      
      //最后执行读取操作 
      case 'R':
      case 'r':
        
        ptr = (uint8*)FLASH_TEST_ADDR;
        
        for( len = 0 ; len < DATA_SIZE ; len++)
        {
            read_buffer[len] = *(ptr + len);
        }
        
        printf("读取数据：\r\n");
        
        LPLD_UART_PutCharArr(UART5, (int8 *)read_buffer, DATA_SIZE);
        
        printf("\r\n读取成功。\r\n");  
        break;  
      default:break;
    }
  } 
}

/*
 * 延时一段时间
 */
void delay()
{
  uint16 i, n;
  for(i=0;i<30000;i++)
  {
    for(n=0;n<200;n++)
    {
      asm("nop");
    }
  }
}
