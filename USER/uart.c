#include "uart.h"
#include "stm8s.h"
#include "stm8s_clk.h"
#include <stdarg.h>

/* ********************************************
UART2  configured as follow:
  - BaudRate = 4800 baud
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Receive and transmit enabled
 -  Receive interrupt
  - UART2 Clock disabled
*********************************************/
void Uart_Init(void)
{
    UART2_DeInit();
    UART2_Init((u32)9600, \
               UART2_WORDLENGTH_8D, \
               UART2_STOPBITS_1, \
               UART2_PARITY_NO , \
               UART2_SYNCMODE_CLOCK_DISABLE , \
               UART2_MODE_TXRX_ENABLE);
               UART2_ITConfig(UART2_IT_RXNE_OR,ENABLE);
    UART2_Cmd(ENABLE );
}
void UART2_SendByte(u8 data)
{
    UART2_SendData8((unsigned char)data);
    /* Loop until the end of transmission */
    while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET)
    {
        ;
    }
}
void UART2_SendString(u8* Data,u16 len)
{
    u16 i=0;
    for(; i<len; i++)
    {
        UART2_SendByte(Data[i]);
    }
}
u8 UART2_ReceiveByte(void)
{
    u8 USART2_RX_BUF;
    while (UART2_GetFlagStatus(UART2_FLAG_RXNE) == RESET)
    {
        ;
    }
    USART2_RX_BUF=UART2_ReceiveData8();
    return  USART2_RX_BUF;
}
/*
 * ��������fputc
 * ����  ���ض���c�⺯��printf��USART1
 * ����  ����
 * ���  ����
 * ����  ����printf����
 */
 /*
int fputc(int ch, FILE *f)
{
//��Printf���ݷ�������
    UART2_SendData8((unsigned char) ch);
    while (!(UART2->SR & UART2_FLAG_TXE))
    {
        ;
    }
    return (ch);
}
*/
/********************************************************
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����USART1_printf()����
 *******************************************************/
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;
    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }
    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }
    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';
        /* Make the value positive. */
        value *= -1;
    }
    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;
        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }
    /* Null terminate the string. */
    *ptr = 0;
    return string;
} /* NCL_Itoa */
/************************************************************************
 * ��������USART1_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-USARTx ����ͨ��������ֻ�õ��˴���1����USART1
 *           -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *             -...    ��������
 * ���  ����
 * ����  ����
 * ����  ���ⲿ����
 *         ����Ӧ��USART1_printf( "\r\n this is a demo \r\n" );
 *                   USART1_printf(  "\r\n %d \r\n", i );
 *                   USART1_printf(  "\r\n %s \r\n", j );
 ***************************************************************************/
void USART2_printf( uint8_t *Data,...)
{
    const char *s;
    int d;
    char buf[16];
    va_list ap;
    va_start(ap, Data);
    while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
    {
        if ( *Data == 0x5c )  //'\'
        {
            switch ( *++Data )
            {
            case 'r':                                   //�س���
                UART2_SendData8(0x0d);
                Data ++;
                break;
            case 'n':                                   //���з�
                UART2_SendData8(0x0a);
                Data ++;
                break;
            default:
                Data ++;
                break;
            }
        }
        else if ( *Data == '%')
        {
            //
            switch ( *++Data )
            {
            case 's':                       //�ַ���
                s = va_arg(ap, const char *);
                for ( ; *s; s++)
                {
                    UART2_SendData8(*s);
                    while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET)
                    {
                        ;
                    }
                }
                Data++;
                break;
            case 'd': //ʮ����
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++)
                {
                    UART2_SendData8(*s);
                    while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET)
                    {
                        ;
                    }
                }
                Data++;
                break;
            default:
                Data++;
                break;
            }
        } /* end of else if */
        else
        {
            UART2_SendData8(*Data++);
        }
        while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET)
        {
            ;
        }
    }
}
