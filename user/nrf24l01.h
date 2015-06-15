/*************************************************************** 
  * @file    nrf24101.h
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   nrf24101 contorl
 ***************************************************************/

#ifndef __NRF24l01_H
#define __NRF24l01_H

#include "stm32f10x.h"

//功能：spi CE管脚设置
#define SPI_CE_H()   GPIO_SetBits(GPIOA, GPIO_Pin_3)
#define SPI_CE_L()   GPIO_ResetBits(GPIOA, GPIO_Pin_3)

void nrf24l01_init(void);
void nrf24l01_rx_mode(void);
u8 nrf24l01_rx_data(u8 *tx_data_buf);
#endif
