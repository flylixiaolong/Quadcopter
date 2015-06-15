/*************************************************************** 
  * @file    spi.c
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   pid control struct
 ***************************************************************/

/***********************************************
����: spi.c
����: spi��ʼ����spi�����ݶ�д
˵����spi1����ĳ�ʼ��
*************************************************/
#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "spi.h"
/*************************************************

���ƣ�spi_init(void)
���ܣ�spi����1��ʼ��
�����������
�����������
����ֵ��  ��
**************************************************/
void spi_init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
 
  //����SPI1�ܽ�
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
	
  //SPI1����ѡ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //˫��ȫ˫�� 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master; //��ģʽ 
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //���ݴ�С8λ 
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; //ʱ�Ӽ��ԣ�����ʱΪ�� 
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; //��1��������Ч��������Ϊ����ʱ�� 
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; //NSS�ź���������� 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; //16��Ƶ OR [SPI_BaudRatePrescaler_8];
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //��λ��ǰ 
	SPI_InitStructure.SPI_CRCPolynomial = 7; //CRC����ʽ
	SPI_Init(SPI1, &SPI_InitStructure); 
	/* Enable SPI1 */ 
	SPI_Cmd(SPI1, ENABLE);	          
}
/*************************************************

���ƣ�spi_rw_byte(unsigned char dt)
���ܣ�spi�ֽ�д
���������д��ֵ
�����������
����ֵ��  ����ֵ
**************************************************/
u8 spi_rw_byte(u8 data)
{
	/* �� SPI���ͻ������ǿ�ʱ�ȴ� */ 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
	/* ͨ�� SPI2����һ�ֽ����� */ 
	SPI_I2S_SendData(SPI1, data); 
	/* ��SPI���ջ�����Ϊ��ʱ�ȴ� */ 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
	/* Return the byte read from the SPI bus */ 
	return SPI_I2S_ReceiveData(SPI1);
}

/*************************************************

���ƣ�spi_rw_buf(u8 *data_buff, u8 byte_quantity, u8 reg_address, u8 control_byte)
���ܣ�spi���ֽڶ�д
���������
  u8 *data_buff     ����ָ��
	u8 byte_quantity  ��д�ֽ�����
	u8 reg_address    �Ĵ�����ַ
	u8 control_byte   ��д���Ʊ�ʶ
�����������
����ֵ��  ��
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
