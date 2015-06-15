/*************************************************************** 
  * @file    interrupt.h
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   interrupt config
 ***************************************************************/

#include "interrupt.h"

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_CFG(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* 1 bit for pre-emption priority, 3 bits for subpriority */

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 

  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
	
//	//´®¿Ú
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_UART_P;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_UART_S;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
//  /* Configure and enable SPI_MASTER interrupt -------------------------------*/
//  NVIC_InitStructure.NVIC_IRQChannel = SPI_MASTER_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);

//  /* Configure and enable SPI_SLAVE interrupt --------------------------------*/
//  NVIC_InitStructure.NVIC_IRQChannel = SPI_SLAVE_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  NVIC_Init(&NVIC_InitStructure);
}
