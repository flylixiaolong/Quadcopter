#ifndef __motor_H
#define __motor_H

#include "stm32f10x.h"

#define PWM_MOTOR_MIN 0
#define PWM_MOTOR_MED 800
#define PWM_MOTOR_MAX 1500

extern s16 motor1,motor2,motor3,motor4;
typedef struct
{
	s16 m1;
	s16 m2;
	s16 m3;
	s16 m4;
}Motor;
typedef s16 Throttle;

/**
  * @brief  motor init
  * @param  None
  * @retval None
  */
void motor_init(void);

/**
  * @brief  motor control
  * @param  motor1,motor2,motor3,motor4
  * @retval None
  */
void motor_control(s16 motor1, s16 motor2, s16 motor3, s16 motor4);
#endif
