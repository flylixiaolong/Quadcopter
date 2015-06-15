#include "i2c.h"
#include "stm32f10x.h"

/**
  * @brief  i2c init
  * @param  None
  * @retval None
  */
void i2c_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//GPIO������Ϣ
	I2C_InitTypeDef I2C_InitStructure;
	//GPIO config
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);//ʱ��ʹ�ܣ����ʹ��GPIOB�˿�//�˿���ӳ��ʱRCC_APB2Periph_AFIO  													
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//��������������
	//GPIO_Mode_AF_PP Ƭ���������; GPIO_Mode_Out_PP ������ݼĴ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���Ƶ��  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//����GPIO_InitStructure��ʼ��GPIOB�˿ڼĴ���

	//I2C1 config
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);//ʱ��ʹ��
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x03A2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress =
	I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000;
	I2C_Init(I2C1, &I2C_InitStructure);
	//I2C_StructInit(&I2C_InitStructure);ʹ��Ĭ�ϲ�����ʼ��
	/* Enable I2C1 peripheral */
	I2C_Cmd(I2C1, ENABLE);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}

/**
  * @brief  write data
  * @param  data д�������
						num	 ���ݸ���
						addr �����ڲ��Ĵ�����ַ
						saddr������ַ
  * @retval None
  */
void i2c_write(u8 *data,u8 num, u8 saddr, u8 addr)
{
	//��ʼλ
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	//����������ַ
	I2C_Send7bitAddress(I2C1, saddr, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	//���͵�ַ
	I2C_SendData(I2C1, addr);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	//��������
	while(num--)
	{
		I2C_SendData(I2C1, *data++); 
		while(!(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)||I2C_GetFlagStatus(I2C2, I2C_FLAG_AF)));
	}
	
	//ֹͣλ
	I2C_GenerateSTOP(I2C1, ENABLE);
}

/**
  * @brief  read data
  * @param  None
  * @retval None
  */
void i2c_read(u8 *data, u8 num, u8 saddr, u8 addr)
{
	/* ��ʼλ */
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	/* ����������ַ(д)*/
	I2C_Send7bitAddress(I2C1, saddr, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	/*���͵�ַ*/
	I2C_SendData(I2C1, addr);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));	
	
	//��ʼλ
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	//����������ַ
	I2C_Send7bitAddress(I2C1, saddr, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	while (num)
	{
		if(num == 1)
		{
			I2C_AcknowledgeConfig(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);	    //����ֹͣλ
		}
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
		*data++ = I2C_ReceiveData(I2C1);
		num--;
	}
	I2C_AcknowledgeConfig(I2C1, ENABLE);  //����Ӧ��ģʽ
}
