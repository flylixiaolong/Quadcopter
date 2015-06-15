/*************************************************************** 
  * @file    imu.h
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   imu moudle
 ***************************************************************/ 
#ifndef __IMU_H
#define __IMU_H

#include "stm32f10x.h"
#include "mpu6050.h"
#include "math.h"

typedef struct{
	float yaw;
	float pitch;
	float roll;
}Attitude;

typedef struct{
	float x;
	float y;
	float z;
}Vector_XYZ;

 /**
  * @brief  init the imu unit
  * @param  None
  * @retval None
  */
void imu_init(Attitude *act_acttitude);

 /**
  * @brief  filter senser data
  * @param  None
  * @retval None
  */
void imu_filter(Vector_XYZ* acc,Vector_XYZ* gyr);

 /**
  * @brief  caculator attitude
  * @param  acc and gyr data
  * @retval None
  */
void imu_attitude(Vector_XYZ acc, Vector_XYZ gyr, Attitude *act_attitude);

#endif
