/*************************************************************** 
  * @file    timer.h
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   time3 init
 ***************************************************************/

#ifndef __TIMER_H_
#define __TIMER_H_

#include "stm32f10x.h"

void timer_init(void);//用于精确延时
void timer_control(u8 status);
#endif
