/*************************************************************** 
  * @file    nrf24101.c
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   nrf24101 contorl
 ***************************************************************/
 
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include "nrf24l01.h"
#include "spi.h"

/* �Ĵ���ָ�� */ 
#define NRF_READ_REG    0x00  // ���Ĵ���ָ�� 
#define NFR_WRITE_REG   0x20  // д�Ĵ���ָ�� 
#define RD_RX_PLOAD     0x61  // ��ȡ��������ָ�� 
#define WR_TX_PLOAD     0xA0  // д��������ָ�� 
#define FLUSH_TX        0xE1  // ��շ��� FIFOָ�� 
#define FLUSH_RX        0xE2  // ��ս��� FIFOָ�� 
#define REUSE_TX_PL     0xE3  // �����ظ�װ������ָ�� 
#define NOP             0xFF  // ����
 
/* �Ĵ�����ַ */ 
#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ 
#define EN_AA           0x01  // �Զ�Ӧ�������� 
#define EN_RXADDR       0x02  // �����ŵ����� 
#define SETUP_AW        0x03  // �շ���ַ������� 
#define SETUP_RETR      0x04  // �Զ��ط��������� 
#define RF_CH           0x05  // ����Ƶ������ 
#define RF_SETUP        0x06  // �������ʡ����Ĺ������� 
#define NRF_STATUS      0x07  // ״̬�Ĵ��� 
#define OBSERVE_TX      0x08  // ���ͼ�⹦�� 
#define CD              0x09  // ��ַ���            
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ 
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ 
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ 
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ 
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ 
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ 
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ��� 
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ��� 
#define RX_PW_P1        0x12  // ����Ƶ��1�������ݳ��� 
#define RX_PW_P2        0x13  // ����Ƶ��2�������ݳ��� 
#define RX_PW_P3        0x14  // ����Ƶ��3�������ݳ��� 
#define RX_PW_P4        0x15  // ����Ƶ��4�������ݳ��� 
#define RX_PW_P5        0x16  // ����Ƶ��5�������ݳ��� 
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������

#define RX_PLOAD_WIDTH	16
#define RX_DR	        0x40
#define RF_CH_VALUE     0x1b

u8 rx_addr[5] = {0x66, 0x88, 0x68, 0x68, 0x68};

/*************************************************

���ƣ�nrf24l01_init(void)
���ܣ�nrf24l01+��ʼ��
�����������
�����������
����ֵ����
**************************************************/
void nrf24l01_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  spi_init();
  
  nrf24l01_rx_mode(); 

}

/*************************************************

���ƣ�nrf24l01_rx_mode(void)
���ܣ�nrf24l01+����ģʽ��ʼ��
�����������
�����������
����ֵ����
**************************************************/
void nrf24l01_rx_mode(void)
{
  u8 data_buf = 0;

  /* �������״̬ */
  SPI_CE_L();

  /* ����ͨ��0���յ�ַ */
  spi_rw_buff(&rx_addr[0], 5, NFR_WRITE_REG + RX_ADDR_P0, WRITE);

  /* ����ͨ��0��Ч���ݳ��� */
  data_buf = RX_PLOAD_WIDTH;
  spi_rw_buff(&data_buf, 1, NFR_WRITE_REG + RX_PW_P0, WRITE);
  
  /* ���Զ�Ӧ�� */
  data_buf = 0;
  spi_rw_buff(&data_buf, 1, NFR_WRITE_REG + EN_AA, WRITE);

  /* ����ͨ��0���� */
  data_buf = 0x01;
  spi_rw_buff(&data_buf, 1, NFR_WRITE_REG + EN_RXADDR, WRITE);
   
  /* ����Ƶ�� */
  data_buf = RF_CH_VALUE;
  spi_rw_buff(&data_buf, 1, NFR_WRITE_REG + RF_CH, WRITE);
  
  /* 1Mbps */
  data_buf = 0;
  spi_rw_buff(&data_buf, 1, NFR_WRITE_REG + RF_SETUP, WRITE);
  
  /* ����ģʽ 16bit CRC ʹ�ܹܽŴ���*/
  data_buf = 0x0f;
  spi_rw_buff(&data_buf, 1, NFR_WRITE_REG + CONFIG, WRITE);

  /* �������״̬ */
  SPI_CE_H();			 
}
/*************************************************
���ƣ�nrf24l01_rx_data(u8 *rx_data_buf)
���ܣ�nrf24l01+��������
���������
    u8 *rx_data_buf ����ָ��  
������������յ�������
����ֵ����
**************************************************/
u8 nrf24l01_rx_data(u8 *rx_data_buf)
{
  u8 status = 0, data_buf = 0, flag = 0;

  spi_rw_buff(&status, 1, NRF_STATUS, READ);

  if(status & RX_DR)
  {
    spi_rw_buff(rx_data_buf, RX_PLOAD_WIDTH, RD_RX_PLOAD, READ);
    
		/* ���ж�*/
		data_buf = RX_DR;
		spi_rw_buff(&data_buf, 1, NFR_WRITE_REG + NRF_STATUS, WRITE);

		flag = 1;
  }

  return flag;
}
/***************************END OF FILE**********************************************************************/
