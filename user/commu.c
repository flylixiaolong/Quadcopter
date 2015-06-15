#include "commu.h"

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

#define ABS(data) 					((data) > 0 ? (data) : (-(data)))
#define  TIME_OUT  1200

extern Attitude act_attitude;
extern Attitude expt_attitude;
extern Throttle throttle;
u8 data_to_send[32];
u16 time_out = 0;
volatile float yaw_offset = 0, pitch_offset = 0, roll_offset = 0;

 /**
  * @brief  send senser data
  * @param  None
  * @retval None
  */
void data_send_senser(void)
{
	u8 _cnt=0;
	u8 sum = 0;
	u8 i = 0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x02;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=data_buf[0];
	data_to_send[_cnt++]=data_buf[1];
	data_to_send[_cnt++]=data_buf[2];
	data_to_send[_cnt++]=data_buf[3];
	data_to_send[_cnt++]=data_buf[4];
	data_to_send[_cnt++]=data_buf[5];
	
	data_to_send[_cnt++]=data_buf[8];
	data_to_send[_cnt++]=data_buf[9];
	data_to_send[_cnt++]=data_buf[10];
	data_to_send[_cnt++]=data_buf[11];
	data_to_send[_cnt++]=data_buf[12];
	data_to_send[_cnt++]=data_buf[13];
	
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	
	data_to_send[3] = _cnt-4;
	
	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];
	data_to_send[_cnt++] = sum;
	usart_send(data_to_send,_cnt);
}

void data_dend_pwm(void)
{
	u8 _cnt = 0;
	u8 sum = 0;
	u8 i = 0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x06;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=BYTE1(motor1);
	data_to_send[_cnt++]=BYTE0(motor1);
	data_to_send[_cnt++]=BYTE1(motor2);
	data_to_send[_cnt++]=BYTE0(motor2);
	data_to_send[_cnt++]=BYTE1(motor3);
	data_to_send[_cnt++]=BYTE0(motor3);
	data_to_send[_cnt++]=BYTE1(motor4);
	data_to_send[_cnt++]=BYTE0(motor4);
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	
	data_to_send[3] = _cnt-4;
	
	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
	usart_send(data_to_send,_cnt);
}

void data_send_status(void)
{
	u8 _cnt=0;
	u8 i = 0;
	u8 sum = 0;
	vs16 _temp = 0;
	vs32 _temp2 = 0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x01;
	data_to_send[_cnt++]=0;
	
	_temp = (int)(act_attitude.roll*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = (int)(act_attitude.pitch*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = (int)(act_attitude.yaw*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);

//	_temp = ABS((int)(expt_attitude.roll))*100;
//	data_to_send[_cnt++]=BYTE1(_temp);//(Rc_D.ROLL);
//	data_to_send[_cnt++]=BYTE0(_temp);//(Rc_D.ROLL);
//	_temp = ABS((int)(expt_attitude.pitch))*100;
//	data_to_send[_cnt++]=BYTE1(_temp);//(Rc_D.PITCH);
//	data_to_send[_cnt++]=BYTE0(_temp);//(Rc_D.PITCH);	
//	_temp = ABS((int)(expt_attitude.yaw))*100;
//	data_to_send[_cnt++]=BYTE1(_temp);//(Rc_D.YAW);
//	data_to_send[_cnt++]=BYTE0(_temp);//(Rc_D.YAW);
	
	_temp = 0;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp2 = 0;
	data_to_send[_cnt++]=BYTE3(_temp2);
	data_to_send[_cnt++]=BYTE2(_temp2);
	data_to_send[_cnt++]=BYTE1(_temp2);
	data_to_send[_cnt++]=BYTE0(_temp2);
		
//	if(Rc_C.ARMED==0)			data_to_send[_cnt++]=0xA0;	//锁定
//	else if(Rc_C.ARMED==1)		
	data_to_send[_cnt++]=0xA1;
	
	data_to_send[3] = _cnt-4;
	
	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];
	data_to_send[_cnt++]=sum;
	
	usart_send(data_to_send,_cnt);
}

//发送遥控数据
void data_send_rc(void)
{
	u8 _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x03;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=BYTE1(throttle);//(Rc_D.THROTTLE);
	data_to_send[_cnt++]=BYTE0(throttle);//(Rc_D.THROTTLE);
	
	vs16 _temp;
	_temp = ABS((int)(expt_attitude.yaw));
	data_to_send[_cnt++]=BYTE1(_temp);//(Rc_D.YAW);
	data_to_send[_cnt++]=BYTE0(_temp);//(Rc_D.YAW);
	_temp = ABS((int)(expt_attitude.roll));
	data_to_send[_cnt++]=BYTE1(_temp);//(Rc_D.ROLL);
	data_to_send[_cnt++]=BYTE0(_temp);//(Rc_D.ROLL);
	_temp = ABS((int)(expt_attitude.pitch));
	data_to_send[_cnt++]=BYTE1(_temp);//(Rc_D.PITCH);
	data_to_send[_cnt++]=BYTE0(_temp);//(Rc_D.PITCH);

	_temp = (int)(yaw_offset*100);
	data_to_send[_cnt++]=BYTE1(_temp);//(Rc_D.AUX1);
	data_to_send[_cnt++]=BYTE0(_temp);//(Rc_D.AUX1);
	_temp = (int)(pitch_offset*100);
	data_to_send[_cnt++]=BYTE1(_temp);//(Rc_D.AUX2);
	data_to_send[_cnt++]=BYTE0(_temp);//(Rc_D.AUX2);
//	_temp = (int)(roll_offset*100);
//	data_to_send[_cnt++]=BYTE1(_temp);//(Rc_D.AUX3);
//	data_to_send[_cnt++]=BYTE0(_temp);//(Rc_D.AUX3);
	
	data_to_send[_cnt++]=BYTE1(motor1);
	data_to_send[_cnt++]=BYTE0(motor1);
	data_to_send[_cnt++]=BYTE1(motor2);
	data_to_send[_cnt++]=BYTE0(motor2);
	data_to_send[_cnt++]=BYTE1(motor3);
	data_to_send[_cnt++]=BYTE0(motor3);
	data_to_send[_cnt++]=BYTE1(motor4);
	data_to_send[_cnt++]=BYTE0(motor4);
	                           
	data_to_send[3] = _cnt-4;  
	                           
	u8 sum = 0;                
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;

	usart_send(data_to_send,_cnt);
}

/*************************************************

名称：data_recv_rc(u8 *rx_data_buf)
功能：接收遥控器数据
输入参数：无
输出参数：油门控制等信息
返回值：无
**************************************************/
void data_recv_rc(Attitude* expt_attitude, Throttle* throttle)
{
	u16 check = 0;
  u8 i = 0;
	u8 rx_data_buf[16];
  if(nrf24l01_rx_data(rx_data_buf))    
  {
		for (i = 0; i < 15; ++i) 
		{
			check += rx_data_buf[i];
		}
		if((check & 0xff) == rx_data_buf[15])
		{
			//LED_BLINK;
			time_out = 0;
			*throttle = rx_data_buf[0] * rx_data_buf[0] * rx_data_buf[0] * 0.0002
				- 0.0645 * rx_data_buf[0] * rx_data_buf[0] + 10.828 * rx_data_buf[0];//0.0002x3 - 0.0645x2 + 10.828x 
			if(rx_data_buf[1] >= 0x80)
				expt_attitude->yaw = rx_data_buf[1] - 0x80;
			else
				expt_attitude->yaw = -rx_data_buf[1];
			if(rx_data_buf[2] >= 0x80)
				expt_attitude->pitch = rx_data_buf[2] - 0x80;
			else
				expt_attitude->pitch = -rx_data_buf[2];
			if(rx_data_buf[3] >= 0x80)
				expt_attitude->roll = rx_data_buf[3] - 0x80;
			else
				expt_attitude->roll = -rx_data_buf[3];
			//expt_attitude->yaw   = expt_attitude->yaw*0.4;
			expt_attitude->pitch = expt_attitude->pitch*0.1;
			expt_attitude->roll  = expt_attitude->roll*0.1;
//			if(rx_data_buf[1] >= 0x80)
//				expt_attitude->yaw = 0x80 - rx_data_buf[1];
//			else
//				expt_attitude->yaw = rx_data_buf[1];
//			if(rx_data_buf[2] >= 0x80)
//				expt_attitude->pitch = 0x80 - rx_data_buf[2];
//			else
//				expt_attitude->pitch = rx_data_buf[2];
//			if(rx_data_buf[3] >= 0x80)
//				expt_attitude->roll = 0x80 - rx_data_buf[3];
//			else
//				expt_attitude->roll = rx_data_buf[3];
			
			yaw_offset = 0;//(rx_data_buf[4] - 0x40) * 1.0;
			pitch_offset = 0;//(rx_data_buf[5] - 0x40) * 0.25;
			roll_offset = 0;//(rx_data_buf[6] - 0x40) * 0.25;
		}
  }
  else  //接收命令超时
  {
     if(time_out++ > TIME_OUT)
     {
			 time_out = TIME_OUT;
			 *throttle = 0;
			 expt_attitude->yaw   = 0;
			 expt_attitude->pitch = 0;
			 expt_attitude->roll  = 0;
     }
  } 
}
