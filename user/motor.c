/*************************************************************** 
  * @file    main.c
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   motor file
 ***************************************************************/
#include "stm32f10x.h"
#include "motor.h" 

s16 motor1 = 0,motor2 = 0,motor3 = 0,motor4 = 0;

/**
  * @brief  motor init
  * @param  None
  * @retval None
  */
void motor_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//TIMxʱ�������������Ϣ
	TIM_OCInitTypeDef  TIM_OCInitStructure;//TIMxʱ�������������Ϣ
	GPIO_InitTypeDef GPIO_InitStructure;//GPIO������Ϣ
	//GPIO config
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);//ʱ��ʹ�ܣ����ʹ��GPIOB�˿�//�˿���ӳ��ʱRCC_APB2Periph_AFIO  													
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_14 | GPIO_Pin_15;//��������������
	//GPIO_Mode_AF_PP Ƭ���������; GPIO_Mode_Out_PP ������ݼĴ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//����Ϊ�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���Ƶ��  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//����GPIO_InitStructure��ʼ��GPIOB�˿ڼĴ���

	//TIM4 config ,motor1 and motor4 config
	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM4, ENABLE);û��ʹ����ӳ��
	//M1,M4���������TIM4��ʱ���ϣ�TIM4����ͨ�ö�ʱ����APB1
	//APB2����AD,I/O,�߼�TIM,����1
	//APB1����DA,USB,SPI,I2C,CAN,����2345,ͨ��TIM
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//��������ʹ��
	/* TIM4���� */
	//Prescaler = (TIM3CLK / TIM3 counter clock) - 1
	//TIM3 Frequency = TIM3 counter clock/(ARR + 1)
	TIM_TimeBaseStructure.TIM_Prescaler = 20 - 1;//ʱ��Ƶ�ʵĳ�����Ԥ��Ƶֵ����0~TIM_Prescaler�����һ��������Ϊ��������
	TIM_TimeBaseStructure.TIM_Period = 2400 - 1;//Ƶ������Ϊ1500HZ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//���ö�ʱ��ģʽ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//���ö�ʱ��ģʽ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_Pulse = 0;//���ô�װ�벶��ȽϼĴ�����ֵ
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);//ͨ��3  
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);//ͨ��4
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable); 

	//TIM1 config ,motor2 and motor3 config
	//M1,M4���������TIM4��ʱ���ϣ�TIM1���ڸ߼���ʱ����APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	/* TIM1���� */		  
	TIM_TimeBaseStructure.TIM_Prescaler = 20 - 1; 
	TIM_TimeBaseStructure.TIM_Period = 2400 - 1;   
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; ; 
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);  
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);  
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	/* TIM1 enable counter */
	TIM_Cmd(TIM1, ENABLE);          
	TIM_Cmd(TIM4, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/**
  * @brief  motor control
  * @param  motor1,motor2,motor3,motor4
  * @retval None
  */
void motor_control(s16 m1, s16 m2, s16 m3, s16 m4)
{
	motor1 = m1, motor2 = m2, motor3 = m3, motor4 = m4;
	motor1=(motor1>PWM_MOTOR_MAX)?PWM_MOTOR_MAX:((motor1<PWM_MOTOR_MIN)?PWM_MOTOR_MIN:motor1);
	motor2=(motor2>PWM_MOTOR_MAX)?PWM_MOTOR_MAX:((motor2<PWM_MOTOR_MIN)?PWM_MOTOR_MIN:motor2);
	motor3=(motor3>PWM_MOTOR_MAX)?PWM_MOTOR_MAX:((motor3<PWM_MOTOR_MIN)?PWM_MOTOR_MIN:motor3);
	motor4=(motor4>PWM_MOTOR_MAX)?PWM_MOTOR_MAX:((motor4<PWM_MOTOR_MIN)?PWM_MOTOR_MIN:motor4);
	
	TIM_SetCompare3(TIM1,motor1);
	TIM_SetCompare3(TIM4,motor2);
	TIM_SetCompare4(TIM4,motor3);
	TIM_SetCompare2(TIM1,motor4);
}
//#define PWM_MOTOR1 TIM1->CCR3   
//#define PWM_MOTOR2 TIM4->CCR3   
//#define PWM_MOTOR3 TIM4->CCR4   
//#define PWM_MOTOR4 TIM1->CCR2 
