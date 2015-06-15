/*************************************************************** 
  * @file    imu.c
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   imu moudle
 ***************************************************************/

#include "imu.h"

#define ACC_ZOOM 8192 //mpu6050 acc +-4g
#define GYR_ZOOM 16.4	//mpu6050 gyr +-2000度/s	
#define TUR_ANGLE 57.295780

#define Kp 15.0f
#define Ki 0.015f//0.015f
#define halfT 0.0005f

volatile Vector_XYZ GYR_OFFSET = {-57.6095,15.0901,7.1200};
//volatile Vector_XYZ GYR_OFFSET = {-57.6095,15.0901,0};
float q0 = 1.0, q1 = 0.0, q2 = 0.0, q3 = 0.0;//初始值
float exInt = 0.0, eyInt = 0.0, ezInt = 0.0;//姿态解算误差的积分
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
			mpu6050_read(&acc,&gyr,&temp);//读取传感器数据,并转换
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
//对加速度和陀螺仪数据进行滤波处理
void imu_filter(Vector_XYZ* acc,Vector_XYZ* gyr)
{
	Senser_XYZ acc_temp,gyr_temp;
	Temp temp;
	mpu6050_read(&acc_temp,&gyr_temp,&temp);//读取传感器数据,并转换
	//加速度数值校正
	acc->x = acc_temp.x;
	acc->y = acc_temp.y;
	acc->z = acc_temp.z;
	//陀螺仪数值校正并量化
	gyr->x = gyr_temp.x-GYR_OFFSET.x;
	gyr->y = gyr_temp.y-GYR_OFFSET.y;
	gyr->z = gyr_temp.z-GYR_OFFSET.z;
//	int i = 0;
//	while(i < 3)
//	{
//		mpu6050_read(&acc_temp,&gyr_temp,&temp);//读取传感器数据,并转换
//		//加速度数值校正
//		acc->x = acc->x + acc_temp.x;
//		acc->y = acc->y + acc_temp.y;
//		acc->z = acc->z + acc_temp.z;
//		//陀螺仪数值校正并量化
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
//计算四轴姿态，用四元数进行姿态估计，用欧拉角控制输出PWM波
void imu_attitude(Vector_XYZ acc, Vector_XYZ gyr, Attitude *act_attitude)
{
	float norm;
	float ex,ey,ez;
	float vx,vy,vz;
	float qa,qb,qc,qd;//暂存q0,q1,q2,q3;
	static float yaw = 0;
	yaw += gyr.z * halfT * 2;
	
  if(!((acc.x == 0.0f) && (acc.y == 0.0f) && (acc.z == 0.0f))) 
  {
		//转换为弧度
		gyr.x = gyr.x / TUR_ANGLE;
		gyr.y = gyr.y / TUR_ANGLE;
		gyr.z = gyr.z / TUR_ANGLE;
		
		//单位化加速度
		norm = sqrt(acc.x*acc.x + acc.y*acc.y + acc.z*acc.z);      
		acc.x = acc.x / norm;
		acc.y = acc.y / norm;
		acc.z = acc.z / norm;	
		
		//把四元数转化为方向余弦矩阵第三列的三个元素，
		//这三个元素组成的向量就是通过陀螺仪积分而来的重力的单位向量在机体坐标系中的坐标
		//将其与加速度计的输出计算而来的重力单位向量(accx,accy,accz)做叉积就得到两者的误差
		//向量。
		vx = 2*(q1*q3 - q0*q2);
		vy = 2*(q0*q1 + q2*q3);
		vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
		
		ex = (acc.y*vz - acc.z*vy);
		ey = (acc.z*vx - acc.x*vz);
		ez = (acc.x*vy - acc.y*vx);
		
		//误差积分
		exInt = exInt + ex*Ki;
		eyInt = eyInt + ey*Ki;
		ezInt = ezInt + ez*Ki;

		//用误差进行PI修正陀螺零偏，通过调节Kp,Ki
		//两个参数可以改变加速度计修正陀螺仪积分姿态的速度
		gyr.x = gyr.x + Kp*ex + exInt;
		gyr.y = gyr.y + Kp*ey + eyInt;
		gyr.z = gyr.z + Kp*ez + ezInt;
	}
	
	qa = q0,qb = q1,qc = q2,qd = q3;
	//采用一阶毕卡（或龙格库塔）公式求解常微分方程
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
	
	//将四元数换算为欧拉角（单位为角度）
	act_attitude->yaw	  =  yaw;//不会造成积分误差太大
	//act_attitude->yaw = - TUR_ANGLE * atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2*q2 - 2 * q3* q3 + 1); // yaw//
	act_attitude->pitch =  TUR_ANGLE * asin(-2 * q1 * q3 + 2 * q0* q2); // pitch
	act_attitude->roll  =  TUR_ANGLE * atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1); // roll
}
