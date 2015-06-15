#include "i2c.h"
#include "stm32f10x.h"

/**
  * @brief  i2c init
  * @param  None
  * @retval None
  */
void i2c_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//GPIO配置信息
	I2C_InitTypeDef I2C_InitStructure;
	//GPIO config
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);//时钟使能，电机使用GPIOB端口//端口重映像时RCC_APB2Periph_AFIO  													
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//待设置设置引脚
	//GPIO_Mode_AF_PP 片上外设输出; GPIO_Mode_Out_PP 输出数据寄存器输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//输出频率  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//根据GPIO_InitStructure初始化GPIOB端口寄存器

	//I2C1 config
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);//时钟使能
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x03A2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress =
	I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000;
	I2C_Init(I2C1, &I2C_InitStructure);
	//I2C_StructInit(&I2C_InitStructure);使用默认参数初始化
	/* Enable I2C1 peripheral */
	I2C_Cmd(I2C1, ENABLE);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}

/**
  * @brief  write data
  * @param  data 写入的数据
						num	 数据个数
						addr 器件内部寄存器地址
						saddr器件地址
  * @retval None
  */
void i2c_write(u8 *data,u8 num, u8 saddr, u8 addr)
{
	//起始位
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	//发送器件地址
	I2C_Send7bitAddress(I2C1, saddr, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	//发送地址
	I2C_SendData(I2C1, addr);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	//发送数据
	while(num--)
	{
		I2C_SendData(I2C1, *data++); 
		while(!(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)||I2C_GetFlagStatus(I2C2, I2C_FLAG_AF)));
	}
	
	//停止位
	I2C_GenerateSTOP(I2C1, ENABLE);
}

/**
  * @brief  read data
  * @param  None
  * @retval None
  */
void i2c_read(u8 *data, u8 num, u8 saddr, u8 addr)
{
	/* 起始位 */
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	/* 发送器件地址(写)*/
	I2C_Send7bitAddress(I2C1, saddr, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	/*发送地址*/
	I2C_SendData(I2C1, addr);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));	
	
	//起始位
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	//发送器件地址
	I2C_Send7bitAddress(I2C1, saddr, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	while (num)
	{
		if(num == 1)
		{
			I2C_AcknowledgeConfig(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);	    //发送停止位
		}
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
		*data++ = I2C_ReceiveData(I2C1);
		num--;
	}
	I2C_AcknowledgeConfig(I2C1, ENABLE);  //允许应答模式
}
