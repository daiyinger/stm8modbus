#include "tim2.h"

void Tim2_Init(void)
{
    TIM2_TimeBaseInit( TIM2_PRESCALER_16 ,20000-1);   //20ms
    TIM2_PrescalerConfig(TIM2_PRESCALER_16,TIM2_PSCRELOADMODE_IMMEDIATE);
    TIM2_ARRPreloadConfig(ENABLE);
    TIM2_ITConfig(TIM2_IT_UPDATE , ENABLE);
    TIM2_Cmd(ENABLE);
}


