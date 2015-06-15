/*************************************************************** 
  * @file    usart.h
  * @author  fly
  * @version V0.1
  * @date    22-April-2015
  * @brief   usart init and send data
 ***************************************************************/
 #ifndef __USART_H_
 #define __USART_H_
 
 #include "stm32f10x.h"
 /**
  * @brief  usart init
  * @param  None
  * @retval None
  */
void usart_init(void);

 /**
  * @brief  usart irq
  * @param  None
  * @retval None
  */	
void usart_irq(void);

 /**
  * @brief  usart send
  * @param  None
  * @retval None
  */
void usart_send(u8 *data, u8 num);

 /**
  * @brief  usart recv
  * @param  None
  * @retval None
  */
u16 usart_recv(u8 *data, u8 num);
void usart_putbuf(u8 *data , u8 num);
void usart_putstr(u8 *str);
#endif
