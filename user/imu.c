/*************************************************************** 
  * @file    imu.c
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   imu moudle
 ***************************************************************/

#include "imu.h"

#define ACC_ZOOM 8192 //mpu6050 acc +-4g
#define GYR_ZOOM 16.4	//mpu6050 gyr +-2000��/s	
#define TUR_ANGLE 57.295780

#define Kp 15.0f
#define Ki 0.015f//0.015f
#define halfT 0.0005f

volatile Vector_XYZ GYR_OFFSET = {-57.6095,15.0901,7.1200};
//volatile Vector_XYZ GYR_OFFSET = {-57.6095,15.0901,0};
float q0 = 1.0, q1 = 0.0, q2 = 0.0, q3 = 0.0;//��ʼֵ
float exInt = 0.0, eyInt = 0.0, ezInt = 0.0;//��̬�������Ļ���
extern u8 flag_update;
s32 gxsum = 0,gysum = 0,gzsum = 0;

 /**
  * @brief  init the imu unit
  * @param  None
  * @retval None
  */
void imu_init(Attitude *act_acttitude)
{
	int i = 0;
	Senser_XYZ acc,gyr;
	Vector_XYZ facc,fgyr;
	Temp temp;
	while(i < 3000)
	{
		if(flag_update == 1)
		{
			flag_update = 0;
			mpu6050_read(&acc,&gyr,&temp);//��ȡ����������,��ת��
			facc.x = acc.x,facc.y = acc.y, facc.z = acc.z;
			fgyr.x = gyr.x / GYR_ZOOM,fgyr.y = gyr.y / GYR_ZOOM,fgyr.z = gyr.z / GYR_ZOOM;
			imu_attitude(facc,fgyr,act_acttitude);
			
			gxsum += gyr.x;
			gysum += gyr.y;
			gzsum += gyr.z;
			
			i++;
		}
	}
	GYR_OFFSET.x = gxsum / 3000;
	GYR_OFFSET.y = gysum / 3000;
	GYR_OFFSET.z = gzsum / 3000;
}

 /**
  * @brief  filter senser data
  * @param  None
  * @retval None
  */
//�Լ��ٶȺ����������ݽ����˲�����
void imu_filter(Vector_XYZ* acc,Vector_XYZ* gyr)
{
	Senser_XYZ acc_temp,gyr_temp;
	Temp temp;
	mpu6050_read(&acc_temp,&gyr_temp,&temp);//��ȡ����������,��ת��
	//���ٶ���ֵУ��
	acc->x = acc_temp.x;
	acc->y = acc_temp.y;
	acc->z = acc_temp.z;
	//��������ֵУ��������
	gyr->x = gyr_temp.x-GYR_OFFSET.x;
	gyr->y = gyr_temp.y-GYR_OFFSET.y;
	gyr->z = gyr_temp.z-GYR_OFFSET.z;
//	int i = 0;
//	while(i < 3)
//	{
//		mpu6050_read(&acc_temp,&gyr_temp,&temp);//��ȡ����������,��ת��
//		//���ٶ���ֵУ��
//		acc->x = acc->x + acc_temp.x;
//		acc->y = acc->y + acc_temp.y;
//		acc->z = acc->z + acc_temp.z;
//		//��������ֵУ��������
//		gyr->x = gyr->x + gyr_temp.x-GYR_OFFSET.x;
//		gyr->y = gyr->y + gyr_temp.y-GYR_OFFSET.y;
//		gyr->z = gyr->z + gyr_temp.z-GYR_OFFSET.z;
//		i++;
//	}
//	acc->x /= 4;
//	acc->y /= 4;
//  acc->z /= 4;
////	gyr->x /= 4;
////	gyr->y /= 4;
////	gyr->z /= 4;	
	gyr->x /= GYR_ZOOM;
	gyr->y /= GYR_ZOOM;
	gyr->z /= GYR_ZOOM;
//	gyr->x = gyr->x / (4 * GYR_ZOOM);
//	gyr->y = gyr->y / (4 * GYR_ZOOM);
//	gyr->z = gyr->z / (4 * GYR_ZOOM);
}
/**
  * @brief  caculator attitude
  * @param  acc and gyr data
  * @retval None
  */
//����������̬������Ԫ��������̬���ƣ���ŷ���ǿ������PWM��
void imu_attitude(Vector_XYZ acc, Vector_XYZ gyr, Attitude *act_attitude)
{
	float norm;
	float ex,ey,ez;
	float vx,vy,vz;
	float qa,qb,qc,qd;//�ݴ�q0,q1,q2,q3;
	static float yaw = 0;
	yaw += gyr.z * halfT * 2;
	
  if(!((acc.x == 0.0f) && (acc.y == 0.0f) && (acc.z == 0.0f))) 
  {
		//ת��Ϊ����
		gyr.x = gyr.x / TUR_ANGLE;
		gyr.y = gyr.y / TUR_ANGLE;
		gyr.z = gyr.z / TUR_ANGLE;
		
		//��λ�����ٶ�
		norm = sqrt(acc.x*acc.x + acc.y*acc.y + acc.z*acc.z);      
		acc.x = acc.x / norm;
		acc.y = acc.y / norm;
		acc.z = acc.z / norm;	
		
		//����Ԫ��ת��Ϊ�������Ҿ�������е�����Ԫ�أ�
		//������Ԫ����ɵ���������ͨ�������ǻ��ֶ����������ĵ�λ�����ڻ�������ϵ�е�����
		//��������ٶȼƵ�������������������λ����(accx,accy,accz)������͵õ����ߵ����
		//������
		vx = 2*(q1*q3 - q0*q2);
		vy = 2*(q0*q1 + q2*q3);
		vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
		
		ex = (acc.y*vz - acc.z*vy);
		ey = (acc.z*vx - acc.x*vz);
		ez = (acc.x*vy - acc.y*vx);
		
		//������
		exInt = exInt + ex*Ki;
		eyInt = eyInt + ey*Ki;
		ezInt = ezInt + ez*Ki;

		//��������PI����������ƫ��ͨ������Kp,Ki
		//�����������Ըı���ٶȼ����������ǻ�����̬���ٶ�
		gyr.x = gyr.x + Kp*ex + exInt;
		gyr.y = gyr.y + Kp*ey + eyInt;
		gyr.z = gyr.z + Kp*ez + ezInt;
	}
	
	qa = q0,qb = q1,qc = q2,qd = q3;
	//����һ�ױϿ����������������ʽ��ⳣ΢�ַ���
	q0 = qa + (-qb*gyr.x - qc*gyr.y - qd*gyr.z)*halfT;
  q1 = qb + ( qa*gyr.x + qc*gyr.z - qd*gyr.y)*halfT;
  q2 = qc + ( qa*gyr.y - qb*gyr.z + qd*gyr.x)*halfT;
  q3 = qd + ( qa*gyr.z + qb*gyr.y - qc*gyr.x)*halfT;
	
	// Normalise quaternion
  norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
  q0 /= norm;
  q1 /= norm;
  q2 /= norm;
  q3 /= norm;
	
	//����Ԫ������Ϊŷ���ǣ���λΪ�Ƕȣ�
	act_attitude->yaw	  =  yaw;//������ɻ������̫��
	//act_attitude->yaw = - TUR_ANGLE * atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2*q2 - 2 * q3* q3 + 1); // yaw//
	act_attitude->pitch =  TUR_ANGLE * asin(-2 * q1 * q3 + 2 * q0* q2); // pitch
	act_attitude->roll  =  TUR_ANGLE * atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1); // roll
}
