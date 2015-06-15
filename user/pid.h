/*************************************************************** 
  * @file    pid.h
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   pid control struct
 ***************************************************************/ 
#ifndef __PID_H_
#define __PID_H_

#include "motor.h"
#include "imu.h"

typedef struct
{
	float kp,ki,kd;
	float pout,iout,dout;
	float error0,error1,error2;
	float delta;
}PID;

enum {angle,yaw,pitch,roll};
void pid_set(PID* pid, float kp, float ki, float kd);
void pid_init(void);
void pid_outer(Attitude e_attitude, Attitude a_attitude);//外环计算角度误差
void pid_inner(Vector_XYZ gyr);
void pid_control(Attitude e_attitude, Attitude a_attitude, Vector_XYZ gyr, Throttle throttle);

#endif
