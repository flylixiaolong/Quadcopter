/*************************************************************** 
  * @file    spi.h
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   pid control struct
 ***************************************************************/

#ifndef __spi_H
#define __spi_H

#include "stm32f10x.h"

//功能：spi nss管脚设置
#define SPI_CSN_H()  GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define SPI_CSN_L()  GPIO_ResetBits(GPIOA, GPIO_Pin_4)

typedef enum {
	WRITE,
	READ,
}CONTORL;

void spi_init(void);
u8	 spi_rw_byte(u8 data);
void spi_rw_buff(u8 *data_buff, u8 byte_quantity, u8 reg_address, CONTORL control_Byte);
#endif
