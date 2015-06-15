/*************************************************************** 
				__________  __	 __	   __
			 / /_______/ / /   \ \	/ /
			/ /					/ /		  \ \/ /
		 / /________ / /			 \/ /
		/ /________// /				 / /
	 / /				 / /_______ / /
	/_/					/_________//_/
	
	* @file    main.c
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   main file
 ***************************************************************/ 
 
#include "stm32f10x.h"

#include "led.h"
#include "motor.h"
#include "i2c.h"
#include "mpu6050.h"
#include "usart.h"
#include "commu.h"
#include "imu.h"
#include "pid.h"
#include "interrupt.h"
#include "timer.h"
#include "nrf24l01.h"

Attitude expt_attitude = {0.0, 0.0, 0.0};//期望姿态，由接收到的遥控数据决定
Attitude act_attitude  = {0.0, 0.0, 0.0};//期望姿态,由计算而来
Throttle throttle = 0;

Vector_XYZ acc = {0.0,0.0,0.0}, gyr = {0.0,0.0,0.0};//加速度计和陀螺仪量化后的数据

u8 flag_update = 0;//跟新标志
int i = 0;//控制LED闪烁

int main(void)
{
	led_init();
	motor_init();
	i2c_init();
	usart_init();
	mpu6050_init();
	timer_init();
	nrf24l01_init();
	NVIC_CFG();
	pid_init();
	delay(5000);
	imu_init(&act_attitude);

	while (1)
	{
		if(flag_update == 1)
		{
			flag_update = 0;
			i++;
			imu_filter(&acc,&gyr);
			imu_attitude(acc,gyr,&act_attitude);
			pid_control(expt_attitude,act_attitude,gyr,throttle);
			data_recv_rc(&expt_attitude,&throttle);
			
			if(i==100)//发送数据到上位机
			{
				i = 0;
				GPIOA->ODR ^= (1<<1);//PA1取反
				data_send_senser();
				data_dend_pwm();
				data_send_status();
				data_send_rc();
			}
		}
	}
}

/*使用断言确定参数出错的位置*/
#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 
  printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}

#endif
