/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_MMA8451）-----------------
 * @file LPLD_MMA8451.c
 * @version 0.1
 * @date 2013-9-29
 * @brief 利用I2C模块，读取MMA8451三轴加速度传感器的值。
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
//包含MMA8451设备头文件
#include "DEV_MMA8451.h"

/****************************************
 说明：
   *将MiniUSB线插入RUSH Kinetis开发板的USB
    插座，并连接至电脑USB接口。
   *使用串口调试助手波特率设置为115200
   *使用串口调试助手查看运行结果
   *摆动RUSH开发板/MMA8451来观察加速度输出值的变化
 ****************************************/

//变量定义
int16 resultx, resulty, resultz;
uint8 device_id;

void main (void)
{

  //初始化MMA8451底层驱动及寄存器
  device_id = LPLD_MMA8451_Init();
  //通过设备ID判断设备是否为MMA8451
  if(device_id == MMA8451_DEV_ID)
  {
    printf("MMA8451 init succeed!\r\n");
    printf("Device ID: 0x%X\r\n", device_id);
  }
  else
  {
    printf("MMA8451 init failed!\r\n");
    printf("Device ID: 0x%X\r\n", device_id);
    while(1);
  }
  
  while(1)
  {  
    resultx = LPLD_MMA8451_GetResult(MMA8451_STATUS_X_READY, MMA8451_REG_OUTX_MSB);
    resulty = LPLD_MMA8451_GetResult(MMA8451_STATUS_Y_READY, MMA8451_REG_OUTY_MSB);
    resultz = LPLD_MMA8451_GetResult(MMA8451_STATUS_X_READY, MMA8451_REG_OUTZ_MSB);
    printf("XYZ:\t%d\t%d\t%d\r\n", resultx, resulty, resultz);

    LPLD_LPTMR_DelayMs(100);
  } 
}

