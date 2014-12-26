#include "stm8s.h"
#include "stm8s_clk.h"
#include "tim2.h"
//#include "intrinsics.h"

#include "mb.h"
#include "mbrtu.h"


//  u8 a[10] = {1,2,3,4,5,6,7,8,9,10}; 
//  eMBRTUSend(1, a, 10);


void main()
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);  //设置内部高速时钟16M为主时钟
    Tim2_Init();
    TIM2_Cmd(ENABLE);
    
    
//  ITC_SetSoftwarePriority(ITC_IRQ_UART2_RX, ITC_PRIORITYLEVEL_3);
//  ITC_SetSoftwarePriority(ITC_IRQ_TIM2_OVF, ITC_PRIORITYLEVEL_2);
//  ITC_SetSoftwarePriority(ITC_IRQ_TIM3_OVF, ITC_PRIORITYLEVEL_1);
    
	enableInterrupts();
    //__enable_interrupt();     // 开启中断     
  
    eMBInit(MB_RTU, 1, 1, 115200, MB_PAR_NONE);
    eMBEnable();
    
    while(1)
    {     
        eMBPoll();
    }
}








#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* Infinite loop */
    while (1)
    {
    }
}
#endif
