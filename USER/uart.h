#ifndef __UART_H
#define __UART_H
#include "stm8s.h"
#include <stdio.h>
/* Private macro -------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))
#define RxBufferSize 64
void Uart_Init(void);
void UART2_SendByte(u8 data);
//int fputc(int ch, FILE *f);
void UART2_SendString(u8* Data,u16 len);
u8 UART2_ReceiveByte(void);
void USART2_printf( uint8_t *Data,...);
#endif
