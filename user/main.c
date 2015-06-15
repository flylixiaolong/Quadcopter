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

Attitude expt_attitude = {0.0, 0.0, 0.0};//������̬���ɽ��յ���ң�����ݾ���
Attitude act_attitude  = {0.0, 0.0, 0.0};//������̬,�ɼ������
Throttle throttle = 0;

Vector_XYZ acc = {0.0,0.0,0.0}, gyr = {0.0,0.0,0.0};//���ٶȼƺ������������������

u8 flag_update = 0;//���±�־
int i = 0;//����LED��˸

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
			
			if(i==100)//�������ݵ���λ��
			{
				i = 0;
				GPIOA->ODR ^= (1<<1);//PA1ȡ��
				data_send_senser();
				data_dend_pwm();
				data_send_status();
				data_send_rc();
			}
		}
	}
}

/*ʹ�ö���ȷ�����������λ��*/
#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 
  printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}

#endif
