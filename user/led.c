#include "led.h"
#include "stm32f10x_conf.h"

/**
  * @brief  init led
  * @param  None
  * @retval None
  */
	
GPIO_InitTypeDef GPIO_InitStructure;	

void led_init(void)
{
	//使能时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	//按键设置为输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	GPIO_ResetBits(GPIOB, GPIO_Pin_8);
	GPIO_ResetBits(GPIOB, GPIO_Pin_9);
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

void delay(u32 count)
{
  for(; count != 0; count--);
}

/**
  * @brief  led flash
  * @param  None
  * @retval None
  */
void led_flash(void)
{
  u8 j;
	for(j = 0; j < 30; j++)
	{
		GPIOA->ODR ^= (1<<1);//PA1取反
		delay(0x80000);
	}
}
