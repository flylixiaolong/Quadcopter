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

	//����USART1
	USART_InitStructure.USART_BaudRate = 115200;       //�����ʿ���ͨ������վ����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8λ����
	USART_InitStructure.USART_StopBits = USART_StopBits_1;   //��֡��β����1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;    //������żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ʧ��
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //���͡�����ʹ��

	//����USART1ʱ��
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;  //ʱ�ӵ͵�ƽ�
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;  //SLCK������ʱ������ļ���->�͵�ƽ
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;  //ʱ�ӵڶ������ؽ������ݲ���
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable; //���һλ���ݵ�ʱ�����岻��SCLK���

	USART_Init(USART1, &USART_InitStructure);
	USART_ClockInit(USART1, &USART_ClockInitStructure);

	//ʹ��USART1�����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//ʹ��USART1
	USART_Cmd(USART1, ENABLE); 
}

//�����жϺ�����stm32f10x_it.c�е���
void usart_irq(void)
{
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART1);
	}
		
	//�����ж�
	if((USART1->SR & (1<<7))&&(USART1->CR1 & USART_CR1_TXEIE))//if(USART_GetITStatus(USART1,USART_IT_TXE)!=RESET)
	{
		USART1->DR = TxBuffer[TxCounter++]; //дDR����жϱ�־          
		if(TxCounter == count)
		{
			USART1->CR1 &= ~USART_CR1_TXEIE;		//�ر�TXE�ж�
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
	//�ж�Strָ��������Ƿ���Ч.
	while(*str)
	{
		//�Ƿ��ǻس��ַ� �����,������Ӧ�Ļس� 0x0d 0x0a
		if(*str=='\r')
			usart_putch(0x0d);
		else if(*str=='\n')
			usart_putch(0x0a);
		else 
			usart_putch(*str);
		//ָ��++ ָ����һ���ֽ�.
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
