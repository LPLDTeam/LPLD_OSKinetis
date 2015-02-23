/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_MPU6050）-----------------
 * @file LPLD_MPU6050.c
 * @version 0.0
 * @date 2013-9-29
 * @brief 关于该工程的描述
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
 * 十分感谢该文件作者对底层库的贡献！
 * http://www.lpld.cn
 * mail:support@lpld.cn
 * 
 * 文件出处：LPLD_K60底层库群
 * 作者：投笔从戎
 *
 */
#include "common.h"
#include "DEV_MPU6050.h"

int16 accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z;
float32 temperature;

void main()
{
  MPU6050_Init();
  
  while(1)
  {
    accel_x = MPU6050_GetResult(ACCEL_XOUT_H);
    accel_y = MPU6050_GetResult(ACCEL_YOUT_H);
    accel_z = MPU6050_GetResult(ACCEL_ZOUT_H);
    gyro_x  = MPU6050_GetResult(GYRO_XOUT_H);
    gyro_y  = MPU6050_GetResult(GYRO_YOUT_H);
    gyro_z  = MPU6050_GetResult(GYRO_ZOUT_H);
    temperature = MPU6050_GetResult(TEMP_OUT_H)/340+36.53;
  } 
}

