/*************************************************************** 
  * @file    timer.c
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   time3 init
 ***************************************************************/

#include "timer.h"

u32 TIM3_IRQCNT = 0;

//1ms中断一次,计数器为1000		
void timer_init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 1000 - 1;    //1ms,1000HZ
  TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;//计数频率1MHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* TIM3 enable counter */
  TIM_Cmd(TIM2, ENABLE);

  /* TIM IT enable */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void timer_control(u8 status)
{
	if(status==0)
		TIM_Cmd(TIM2,DISABLE);
	if(status==1)
		TIM_Cmd(TIM2,ENABLE);
}
