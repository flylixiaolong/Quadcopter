/*************************************************************** 
  * @file    led.h
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   led control using GPIO Pin11 PA1
 ***************************************************************/ 
#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define  LED_BLINK  GPIOA->ODR ^= (1 << 1)

void delay(u32 count);
void led_init(void);
void led_flash(void);

#endif
