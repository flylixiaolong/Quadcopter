/*************************************************************** 
  * @file    MCU6050.h
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   led control using GPIO Pin11 PA1
 ***************************************************************/ 
#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f10x.h"

typedef struct{
	s16 x;
	s16 y;
	s16 z;
}Senser_XYZ;

typedef u16 Temp;

extern u8 data_buf[14];//接收从MPU6050传送来的数据

void mpu6050_init(void);
void mpu6050_read(Senser_XYZ* acc, Senser_XYZ* gyr, Temp* temp);

#endif
