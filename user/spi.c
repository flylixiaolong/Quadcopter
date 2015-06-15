/*************************************************************** 
  * @file    spi.c
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   pid control struct
 ***************************************************************/

/***********************************************
标题: spi.c
功能: spi初始化以spi及数据读写
说明：spi1外设的初始化
*************************************************/
#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "spi.h"
/*************************************************

名称：spi_init(void)
功能：spi外设1初始化
输入参数：无
输出参数：无
返回值：  无
**************************************************/
void spi_init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
 
  //配置SPI1管脚
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	//NSS
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//GPIO_Pin_3 | 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	SPI_CSN_H();
	
  //SPI1配置选项
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //双线全双工 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master; //主模式 
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //数据大小8位 
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; //时钟极性，空闲时为低 
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; //第1个边沿有效，上升沿为采样时刻 
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; //NSS信号由软件产生 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; //16分频 OR [SPI_BaudRatePrescaler_8];
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //高位在前 
	SPI_InitStructure.SPI_CRCPolynomial = 7; //CRC多项式
	SPI_Init(SPI1, &SPI_InitStructure); 
	/* Enable SPI1 */ 
	SPI_Cmd(SPI1, ENABLE);	          
}
/*************************************************

名称：spi_rw_byte(unsigned char dt)
功能：spi字节写
输入参数：写入值
输出参数：无
返回值：  读出值
**************************************************/
u8 spi_rw_byte(u8 data)
{
	/* 当 SPI发送缓冲器非空时等待 */ 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
	/* 通过 SPI2发送一字节数据 */ 
	SPI_I2S_SendData(SPI1, data); 
	/* 当SPI接收缓冲器为空时等待 */ 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
	/* Return the byte read from the SPI bus */ 
	return SPI_I2S_ReceiveData(SPI1);
}

/*************************************************

名称：spi_rw_buf(u8 *data_buff, u8 byte_quantity, u8 reg_address, u8 control_byte)
功能：spi多字节读写
输入参数：
  u8 *data_buff     数据指针
	u8 byte_quantity  读写字节数量
	u8 reg_address    寄存器地址
	u8 control_byte   读写控制标识
输出参数：无
返回值：  无
**************************************************/
void spi_rw_buff(u8 *data_buff, u8 byte_quantity, u8 reg_address, CONTORL control_byte)
{
  u8 i;
  if(control_byte == 0)  //write
  { 		 
    SPI_CSN_L();

		spi_rw_byte(reg_address);

		for(i = 0; i < byte_quantity; i++)
		{
			spi_rw_byte(*data_buff);
			data_buff++;
		}

		SPI_CSN_H();
  }
  else
  {
    SPI_CSN_L();

		spi_rw_byte(reg_address);

		for(i = 0; i < byte_quantity; i++)
		{
			*data_buff = spi_rw_byte(0);
			data_buff++;
		}

		SPI_CSN_H();
  }
}
