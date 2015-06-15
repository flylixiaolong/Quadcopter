/*************************************************************** 
  * @file    mpu6050.c
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   led control using GPIO Pin11 PA1
 ***************************************************************/
 
#include "mpu6050.h"
#include "stm32f10x.h"
#include "i2c.h"

//MPU2050寄存器地址
#define MPU6050_ADDR       0xd0    //AD0 = 0 时地址
#define CONFIG             0x1a
#define GYRO_CONFIG        0x1b
#define ACCEL_CONFIG       0x1c	
#define INT_PIN_CFG        0x37 
#define MPU6050_BURST_ADDR 0x3b 
#define USER_CTLR          0x6a	       
#define PWR_MGMT1          0x6b	     
#define PWR_MGMT2          0x6c
#define MPU6050_ID_ADDR	   0x75
#define MPU6050_ID	       0x68
#define MPU6050_RA_SMPLRT_DIV       0x19

u8 data_buf[14];//接收从MPU6050传送来的数据

void mpu6050_init(void)
{
	u8 data = 0;

	/* i2c bypass en */
	data = 0x02;
	i2c_write(&data, 1, MPU6050_ADDR, INT_PIN_CFG);

	/* iic master disable *///配置为从机模式
	data  =0x00;
	i2c_write(&data, 1, MPU6050_ADDR, USER_CTLR);

	/* mpu6050 sleep disable, temperature en, in 8M osc *///电源模式和时钟源选择
	data = 0x00;
	i2c_write(&data, 1, MPU6050_ADDR, PWR_MGMT1);

	/* mpu6050 no standby mode *///全部不进入待机模式
	data = 0x00;
	i2c_write(&data, 1, MPU6050_ADDR, PWR_MGMT2);

	/* DLPF *///滤波器
	data = 0x06;
	i2c_write(&data, 1, MPU6050_ADDR, CONFIG);

	/* GYRO +-2000 °/s *///陀螺仪刻度范围
	data = 0x18;
	i2c_write(&data, 1, MPU6050_ADDR, GYRO_CONFIG);

	/* ACC +-4g */
	data = 0x08;//加速度计输出范围
	i2c_write(&data, 1, MPU6050_ADDR, ACCEL_CONFIG);
}

void mpu6050_read(Senser_XYZ* acc, Senser_XYZ* gyr, Temp* temp)
{
  i2c_read(&data_buf[0], 14, MPU6050_ADDR, MPU6050_BURST_ADDR);
	acc->x = data_buf[0] * 0x100 + data_buf[1];
	acc->y = data_buf[2] * 0x100 + data_buf[3];
	acc->z = data_buf[4] * 0x100 + data_buf[5];
	*temp  = data_buf[6] * 0x100 + data_buf[7];
	gyr->x = data_buf[8] * 0x100 + data_buf[9];
	gyr->y = data_buf[10] * 0x100 + data_buf[11];
	gyr->z = data_buf[12] * 0x100 + data_buf[13];
}
