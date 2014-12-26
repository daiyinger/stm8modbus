#include "port.h"
#include "mb.h"
#include "mbport.h"
#include "stm8s.h"
#include "stm8s_uart2.h"
#include "stm8s_tim1.h"


static eMBEventType eQueuedEvent;
static BOOL xEventInQueue;


/* ----------------------- Prototypes ---------------------------------------*/
void vMBPortEnterCritical( void )
{
  disableInterrupts();    /* disable interrupts */  
}



void vMBPortExitCritical( void )
{
  enableInterrupts();     /* enable interrupts */ 
}



/* ----------------------- Supporting functions -----------------------------*/

BOOL xMBPortEventInit( void )
{
    xEventInQueue = FALSE;
    return TRUE;
}




BOOL xMBPortEventPost( eMBEventType eEvent )
{
    xEventInQueue = TRUE;
    eQueuedEvent = eEvent;
    return TRUE;
}




BOOL xMBPortEventGet( eMBEventType * eEvent )
{
    BOOL xEventHappened = FALSE;

    if( xEventInQueue )
    {
        *eEvent = eQueuedEvent;
        xEventInQueue = FALSE;
        xEventHappened = TRUE;
    }
    return xEventHappened;
}





/* ----------------------- Serial port functions ----------------------------*/

BOOL xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate,UCHAR ucDataBits, eMBParity eParity )
{
  BOOL bInitialized = TRUE;
  u8 WordLend = 0;
  u8 Patity = 0;

  ucPort = ucPort;
    
  UART2_DeInit();  

  switch (eParity)
  {
    case MB_PAR_NONE:   Patity = UART2_PARITY_NO;   break;
    case MB_PAR_ODD:    Patity = UART2_PARITY_ODD;  break;
    case MB_PAR_EVEN:   Patity = UART2_PARITY_EVEN; break;
  }
  
  switch (ucDataBits)
  {
    case 8:
	if(eParity == MB_PAR_NONE)
          WordLend = UART2_WORDLENGTH_8D; 
	else
	  WordLend = UART2_WORDLENGTH_9D; 
	break;
    case 7:
	break;
    default:
	bInitialized = FALSE;
  }
  
  if(bInitialized)
  {
    ENTER_CRITICAL_SECTION(  );
    UART2_Init(ulBaudRate, \
               WordLend, \
               UART2_STOPBITS_1, \
               Patity, \
               UART2_SYNCMODE_CLOCK_DISABLE, \
               UART2_MODE_TXRX_ENABLE);
    //UART2_ITConfig(UART2_IT_RXNE_OR,ENABLE);
    UART2_Cmd(ENABLE);
    EXIT_CRITICAL_SECTION(  );
  }
  return bInitialized;
}




void UART2_TXD_IRQ(void)
{
  if(UART2_GetITStatus(UART2_IT_TXE) == SET)
  {	    
    UART2->SR &= 0xBF;  //clear UART2_SR_TC register
    pxMBFrameCBTransmitterEmpty();    
  }
}



void UART2_RXD_IRQ(void)
{
  if(UART2_GetITStatus(UART2_IT_RXNE) == SET)
  {
    UART2->SR &= 0xDF;  //clear UART2_SR_RXNE register
    pxMBFrameCBByteReceived();      
  }
}




void vMBPortClose( void )
{
    xMBPortSerialClose(  );
    xMBPortTimersClose(  );  
}






void xMBPortSerialClose( void )
{
  UART2_Cmd(DISABLE);  
}






void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    ENTER_CRITICAL_SECTION();
    
    if(xRxEnable)
    {
        UART2_ITConfig(UART2_IT_RXNE_OR, ENABLE);   //开启接收
    }
    else
    {
       UART2_ITConfig(UART2_IT_RXNE_OR, DISABLE);
    }
    
    if(xTxEnable)
    {

       UART2_ITConfig(UART2_IT_TXE, ENABLE);      //开启发送
    }
    else
    {
       UART2_ITConfig(UART2_IT_TXE, DISABLE);
    }
    
    EXIT_CRITICAL_SECTION(  );  
}





BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
    //while (UART2_GetFlagStatus(UART2_FLAG_RXNE) == RESET)
    //{
    //    ;
    //}
    * pucByte = UART2_ReceiveData8();
    return  TRUE;  
}





BOOL xMBPortSerialPutByte( CHAR ucByte )
{
    UART2_SendData8(ucByte);
    /* Loop until the end of transmission */
    //while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET)
    //{
    //    ;
    //}  
    return TRUE;
}





/* ----------------------- Timers functions ---------------------------------*/
BOOL xMBPortTimersInit( USHORT usTimeOut50us )
{
    TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
    TIM1_TimeBaseInit(799, TIM1_COUNTERMODE_UP, usTimeOut50us, 0);   //TIM1的分频系数为，时间基频率为 16M / (1 + Prescaler) = 20KHz, 基准为50us
    TIM1_SetCounter(0x0000);
    TIM1_ARRPreloadConfig(ENABLE);
    TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
  
    return TRUE;
}




void xMBPortTimersClose( void )
{
    TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
    TIM1_SetCounter(0x0000);
    TIM1_ARRPreloadConfig(DISABLE);
    TIM1_ITConfig(TIM1_IT_UPDATE, DISABLE);
    TIM1_Cmd(DISABLE);  
}




void vMBPortTimersEnable( void )
{
    TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
    TIM1_SetCounter(0x0000);
    TIM1_ARRPreloadConfig(ENABLE);
    TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
    TIM1_Cmd(ENABLE);
}




void vMBPortTimersDisable( void )
{
  TIM1_Cmd(DISABLE);   
}




void vMBPortTimersDelay( USHORT usTimeOutMS )   //ms delay
{
  unsigned char i;
  while(usTimeOutMS!=0)
  {
    for(i=0;i<250;i++)
      { }
      for(i=0;i<75;i++)
      { }
       usTimeOutMS--;
  }  
}












