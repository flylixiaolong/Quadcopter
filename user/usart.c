/*************************************************************** 
  * @file    usart.c
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   usart init and send data
 ***************************************************************/

 #include "usart.h"
 
u8 TxBuffer[256];
u8 TxCounter=0;
u8 count=0;
static u8 RxBuffer[50];
static u8 RxState = 0;

 /**
  * @brief  usart init
  * @param  None
  * @retval None
  */
void usart_init(void)
{
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* Enable USART1 Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Configure USARTy TX as push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USARTy RX as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//配置USART1
	USART_InitStructure.USART_BaudRate = 115200;       //波特率可以通过地面站配置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8位数据
	USART_InitStructure.USART_StopBits = USART_StopBits_1;   //在帧结尾传输1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;    //禁用奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //发送、接收使能

	//配置USART1时钟
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;  //时钟低电平活动
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;  //SLCK引脚上时钟输出的极性->低电平
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;  //时钟第二个边沿进行数据捕获
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable; //最后一位数据的时钟脉冲不从SCLK输出

	USART_Init(USART1, &USART_InitStructure);
	USART_ClockInit(USART1, &USART_ClockInitStructure);

	//使能USART1接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//使能USART1
	USART_Cmd(USART1, ENABLE); 
}

//串口中断函数在stm32f10x_it.c中调用
void usart_irq(void)
{
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART1);
	}
		
	//发送中断
	if((USART1->SR & (1<<7))&&(USART1->CR1 & USART_CR1_TXEIE))//if(USART_GetITStatus(USART1,USART_IT_TXE)!=RESET)
	{
		USART1->DR = TxBuffer[TxCounter++]; //写DR清除中断标志          
		if(TxCounter == count)
		{
			USART1->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE中断
			//USART_ITConfig(USART1,USART_IT_TXE,DISABLE);
		}
	}
}

 /**
  * @brief  usart send
  * @param  None
  * @retval None
  */
void usart_send(u8 *data, u8 num)
{
	u8 i = 0;
	while(i < num)
	{
		USART_SendData(USART1, data[i++]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
//		for(u8 i=0;i<data_num;i++)
//		TxBuffer[count++] = *(DataToSend+i);
//	if(!(USART1->CR1 & USART_CR1_TXEIE))
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
}

 /**
  * @brief  usart recv
  * @param  None
  * @retval None
  */
u16 usart_recv(u8 *data, u8 num)
{
	return USART_ReceiveData(USART2);
}

static void usart_putch(unsigned char DataToSend)
{
	TxBuffer[count++] = DataToSend;  
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
}
void usart_putstr(u8 *str)
{
	//判断Str指向的数据是否有效.
	while(*str)
	{
		//是否是回车字符 如果是,则发送相应的回车 0x0d 0x0a
		if(*str=='\r')
			usart_putch(0x0d);
		else if(*str=='\n')
			usart_putch(0x0a);
		else 
			usart_putch(*str);
		//指针++ 指向下一个字节.
		str++;
	}
}
void usart_putbuf(u8 *data , u8 num)
{
	for(u8 i=0;i<num;i++)
		TxBuffer[count++] = *(data+i);
	if(!(USART1->CR1 & USART_CR1_TXEIE))
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
}
