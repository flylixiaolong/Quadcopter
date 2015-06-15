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
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//TIMx时间基数的配置信息
	TIM_OCInitTypeDef  TIM_OCInitStructure;//TIMx时间基数的配置信息
	GPIO_InitTypeDef GPIO_InitStructure;//GPIO配置信息
	//GPIO config
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);//时钟使能，电机使用GPIOB端口//端口重映像时RCC_APB2Periph_AFIO  													
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_14 | GPIO_Pin_15;//待设置设置引脚
	//GPIO_Mode_AF_PP 片上外设输出; GPIO_Mode_Out_PP 输出数据寄存器输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//设置为推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//输出频率  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//根据GPIO_InitStructure初始化GPIOB端口寄存器

	//TIM4 config ,motor1 and motor4 config
	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM4, ENABLE);没有使用重映像
	//M1,M4电机挂载在TIM4定时器上，TIM4属于通用定时器，APB1
	//APB2控制AD,I/O,高级TIM,串口1
	//APB1控制DA,USB,SPI,I2C,CAN,串口2345,通用TIM
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//控制引脚使能
	/* TIM4设置 */
	//Prescaler = (TIM3CLK / TIM3 counter clock) - 1
	//TIM3 Frequency = TIM3 counter clock/(ARR + 1)
	TIM_TimeBaseStructure.TIM_Prescaler = 20 - 1;//时钟频率的除数，预分频值。从0~TIM_Prescaler，输出一个脉冲做为计数脉冲
	TIM_TimeBaseStructure.TIM_Period = 2400 - 1;//频率设置为1500HZ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//设置定时器模式
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//设置定时器模式
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出比较极性高
	TIM_OCInitStructure.TIM_Pulse = 0;//设置待装入捕获比较寄存器的值
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);//通道3  
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);//通道4
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable); 

	//TIM1 config ,motor2 and motor3 config
	//M1,M4电机挂载在TIM4定时器上，TIM1属于高级定时器，APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	/* TIM1设置 */		  
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
