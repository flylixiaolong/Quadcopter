/*************************************************************** 
  * @file    pid1.c
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   pid control struct
 ***************************************************************/
 
#include "pid.h"
#include "motor.h"

PID pid[4];
static float factor[4][3] = {   //PID         			kp ki kd
	{0.6,  0.0,  10.0},						  	//angle pid                {1.2,  0.0,   0.0},	
	{19.5,  0.01,  80.0}, 							//yaw   pid//12.4,0.1,160  {16.4, 0.0,   160.0}, 
	{1.8,  0.01,  80.0}, 								//pitch pid0.005           {1.2,  0.01,  60.0}, 	
	{1.8,  0.01,  80.0} 							  //roll  pid                {1.2,  0.01,  60.0} 	
};
static float q[4][3];   				//��PIDת��Ϊϵ��		q0 q1 q2
          											//angle 		-20.0
          											//yaw       -1.6 
          											//pitch     -1.6 
          											//roll      -1.6          

Attitude error[3] = {{0.0,0,0},{0.0,0.0,0},{0.0,0.0,0.0}};//��ǰ���ϴ��������ϴ����
Attitude erate = {0.0,0.0,0.0};//�⻷�����

void pid_set(PID* pid, float kp, float ki, float kd)
{
	pid->kp = kp,pid->ki = ki,pid->kd = kd;
	pid->pout = pid->iout = pid->dout = 0.0;
	pid->error0 = pid->error1 = pid->error2 = 0.0;
	pid->delta = 0.0;
}

void pid_reset()
{
	int i=0;
	for(i=0;i<4;i++)
	{
		pid_set(&pid[i],factor[i][0],factor[i][1],factor[i][2]);//factor[3*i],factor[3*i+1],factor[3*i+2]
		error[i].yaw = error[i].pitch = error[i].roll = 0;
	}
}

void pid_fact(float a[4][3], float b[4][3])
{
	int i = 0;
	for(i=0; i<4; i++)
	{
		a[i][0] =  b[i][0] + b[i][1] + b[i][2];
		a[i][1] = -b[i][0] - 2*b[i][2];
		a[i][2] =  b[i][2];
	}
}

void pid_init(void)
{
	int i = 0;
	for(i=0; i<4; i++)
	{
		pid_set(&pid[i],factor[i][0],factor[i][1],factor[i][2]);//factor[3*i],factor[3*i+1],factor[3*i+2]
	}
	pid_fact(q,factor);
}

void pid_outer(Attitude e_attitude, Attitude a_attitude)//�⻷����Ƕ����
{
	//�⻷�������ǽǶȲ�
	//�������
//	int i = 1;
	error[0].yaw   = e_attitude.yaw   - (-a_attitude.yaw);
	error[0].pitch = e_attitude.pitch - (-a_attitude.pitch);
	error[0].roll  = e_attitude.roll  - (-a_attitude.roll);

	//����ʽpid�����㷨
	erate.yaw   += q[0][0] * error[0].yaw   + q[0][1] * error[1].yaw   + q[0][2] * error[2].yaw  ;//a_attitude.yaw +   
	erate.pitch += q[0][0] * error[0].pitch + q[0][1] * error[1].pitch + q[0][2] * error[2].pitch;//a_attitude.pitch + 
	erate.roll  += q[0][0] * error[0].roll  + q[0][1] * error[1].roll  + q[0][2] * error[2].roll ;//a_attitude.roll +  

	//�������
	error[2].yaw   = error[1].yaw;
	error[2].pitch = error[1].pitch;
	error[2].roll  = error[1].roll;
	error[1].yaw   = error[0].yaw;
	error[1].pitch = error[0].pitch;
	error[1].roll  = error[0].roll;
	//�����Ϊ�ڻ�������ֵ
}


extern Attitude expt_attitude;
void pid_inner(Vector_XYZ gyr)
{
	//����Ϊ���ٶȲ�
	pid[yaw].error0   = 3.5 * erate.yaw   - (-gyr.z);//expt_attitude.yaw   - (-gyr.z);//
	pid[pitch].error0 = 3.5 * erate.pitch - (-gyr.y);//expt_attitude.pitch - (-gyr.y);//
	pid[roll].error0  = 3.5 * erate.roll  - (-gyr.x);//expt_attitude.roll  - (-gyr.x);//
	
	//ƫ��yaw
	pid[yaw].delta += q[1][0] * pid[yaw].error0 + q[1][1] * pid[yaw].error1 + q[1][2] * pid[yaw].error2;
	//����pitch
	pid[pitch].delta += q[2][0] * pid[pitch].error0 + q[2][1] * pid[pitch].error1 +q[2][2] * pid[pitch].error2;
	//���roll
	pid[roll].delta += q[3][0] * pid[roll].error0 + q[3][1] * pid[roll].error1 + q[3][2] * pid[roll].error2;
	
	//�������
	pid[yaw].error2   = pid[yaw].error1;
	pid[pitch].error2 = pid[pitch].error1;
	pid[roll].error2  = pid[roll].error1;
	pid[yaw].error1   = pid[yaw].error0;
	pid[pitch].error1 = pid[pitch].error0;
	pid[roll].error1  = pid[roll].error0;
}

void pid_control(Attitude e_attitude, Attitude a_attitude, Vector_XYZ gyr, Throttle throttle)
{
	static u16 m1=0,m2=0,m3=0,m4=0;
	//ˮƽʱ��ָ�Ϊ����ת��
//	float a = e_attitude.pitch*e_attitude.pitch + e_attitude.yaw * e_attitude.yaw +  e_attitude.roll * e_attitude.roll;
//	float b = a_attitude.pitch*a_attitude.pitch + a_attitude.yaw * a_attitude.yaw +  a_attitude.roll * a_attitude.roll;
	
	if(throttle != 0)// && a > 0.5 && b > 0.5
	{
		pid_outer(e_attitude,a_attitude);//�⻷����Ƕ����
		pid_inner(gyr);
		//�����ںϿ��Ƶ��
//		m1 = (int16_t)( throttle + pid[pitch].delta + pid[roll].delta + pid[yaw].delta );    //M1+200+ 600
//		m2 = (int16_t)( throttle + pid[pitch].delta - pid[roll].delta - pid[yaw].delta );    //M2+200+ 600
//		m3 = (int16_t)( throttle - pid[pitch].delta - pid[roll].delta + pid[yaw].delta );    //M3+200+ 600
//		m4 = (int16_t)( throttle - pid[pitch].delta + pid[roll].delta - pid[yaw].delta );    //M4+200+ 600
		m1 = (int16_t)( throttle - pid[pitch].delta - pid[roll].delta - pid[yaw].delta );    //M1+200+ 600
		m2 = (int16_t)( throttle - pid[pitch].delta + pid[roll].delta + pid[yaw].delta );    //M2+200+ 600
		m3 = (int16_t)( throttle + pid[pitch].delta + pid[roll].delta - pid[yaw].delta );    //M3+200+ 600
		m4 = (int16_t)( throttle + pid[pitch].delta - pid[roll].delta + pid[yaw].delta );    //M4+200+ 600
	}
	else
	{	
		m1 = m2 = m3 = m4 = 0;
		pid_reset();
	}
	motor_control(m1,m2,m3,m4);
}

