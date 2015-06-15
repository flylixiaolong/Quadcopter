/*************************************************************** 
  * @file    i2c.h
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   i2c init , read and write data
 ***************************************************************/ 
#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x.h"

/**
  * @brief  i2c init
  * @param  None
  * @retval None
  */
void i2c_init(void);

/**
  * @brief  write data
  * @param  None
  * @retval None
  */
void i2c_write(u8 *data,u8 num, u8 saddr, u8 addr);

/**
  * @brief  read data
  * @param  None
  * @retval None
  */
void i2c_read(u8 *data, u8 num, u8 saddr, u8 addr);
#endif
