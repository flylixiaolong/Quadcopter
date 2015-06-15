/*************************************************************** 
  * @file commu.h
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   communicate with pc
 ***************************************************************/
#ifndef __COMMU_H_
#define __COMMU_H_

#include "stm32f10x.h"
#include "i2c.h"
#include "usart.h"
#include "mpu6050.h"
#include "motor.h"
#include "imu.h"
#include "nrf24l01.h"
#include "led.h"
 
 
typedef struct{
	s16 a;
}RC_data;

 /**
  * @brief  send senser data
  * @param  None
  * @retval None
  */
void data_send_senser(void);
void data_dend_pwm(void);
void data_send_status(void);
void data_send_rc(void);
void data_recv_rc(Attitude* expt_attitude, Throttle* throttle);

#endif
