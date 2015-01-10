/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_MAG3110）-----------------
 * @file LPLD_MAG3110.c
 * @version 0.2[By LPLD]
 * @date 2013-10-3
 * @brief 利用I2C模块，读取MAG3110三轴地磁传感器的值。
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
//包含MAG3110设备头文件
#include "DEV_MAG3110.h"

/****************************************
 说明：
   *将MAG3110的SCL引脚连接开发板的PTB0引脚
    将MAG3110的SDA引脚连接开发板的PTB1引脚
    将MAG3110的VCC和GND分别接开发板的3V3和GND
   *将MiniUSB线插入RUSH Kinetis开发板的USB
    插座，并连接至电脑USB接口。
   *使用串口调试助手波特率设置为115200
   *使用串口调试助手查看运行结果
   *摆动MAG3110来观察输出值的变化
 ****************************************/

//变量定义
int16 resultx, resulty, resultz;
uint8 device_id;

void main (void)
{

  //初始化MAG3110底层驱动及寄存器
  device_id = LPLD_MAG3110_Init();
  //通过设备ID判断设备是否为MAG3110
  if(device_id == MAG3110_DEV_ID)
  {
    printf("MAG3110 init succeed!\r\n");
    printf("Device ID: 0x%X\r\n", device_id);
  }
  else
  {
    printf("MAG3110 init failed!\r\n");
    printf("Device ID: 0x%X\r\n", device_id);
    while(1);
  }
  
  while(1)
  {  
    resultx = LPLD_MAG3110_GetResult(DR_STATUS_X_READY, MAG3110_OUT_X_MSB);
    resulty = LPLD_MAG3110_GetResult(DR_STATUS_Y_READY, MAG3110_OUT_Y_MSB);
    resultz = LPLD_MAG3110_GetResult(DR_STATUS_Z_READY, MAG3110_OUT_Z_MSB);
    printf("XYZ:\t%d\t%d\t%d\r\n", resultx, resulty, resultz);

    LPLD_LPTMR_DelayMs(100);
  } 
}

