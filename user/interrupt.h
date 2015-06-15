/*************************************************************** 
  * @file    interrupt.h
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   interrupt config
 ***************************************************************/
#ifndef __INTERRUPT_H
#define __INTERRUPT_H

#include "stm32f10x.h"

#define NVIC_UART_P	5
#define NVIC_UART_S	1
#define NVIC_TIM3_P	2
#define NVIC_TIM3_S	1
#define NVIC_TIM4_P	0
#define NVIC_TIM4_S	1

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_CFG(void);

#endif
