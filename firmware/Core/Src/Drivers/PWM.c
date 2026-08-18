#include "PWM.h"
#include "stm32h7xx.h"

void Init_Channel(TIM_TypeDef *timer, uint8_t channel);

void PWM_Init(void)
{
    // TIM1 Initialization 
    Init_Channel(TIM1, 1);
    Init_Channel(TIM1, 2);

    TIM1->BDTR |= TIM_BDTR_MOE; // Enable Output as an extra step for advanced timers
    TIM1->CR1 |= TIM_CR1_CEN; // Enable Counter

    // TIM2 Initialization
    // Set TIM2 channels polarity to active high and enable respective channels
    Init_Channel(TIM2, 1);
    Init_Channel(TIM2, 2);
    Init_Channel(TIM2, 3);
    Init_Channel(TIM2, 4);

    TIM2->CR1 |= TIM_CR1_CEN; // Enable counter
}

void Init_Channel(TIM_TypeDef *timer, uint8_t channel){
    
    uint64_t CCER_CCxP, CCER_CCxE; 

    if (channel < 1 || channel > 4)
        return;

    if (channel == 1){
        CCER_CCxP = TIM_CCER_CC1P;
        CCER_CCxE = TIM_CCER_CC1E;
    }
    else if (channel == 2){
        CCER_CCxP = TIM_CCER_CC2P;
        CCER_CCxE = TIM_CCER_CC2E;
    }
    else if (channel == 3){
        CCER_CCxP = TIM_CCER_CC3P;
        CCER_CCxE = TIM_CCER_CC3E;
    }
    else if (channel == 4){
        CCER_CCxP = TIM_CCER_CC4P;
        CCER_CCxE = TIM_CCER_CC4E;
    }
    
    timer->CCER &= ~CCER_CCxP; // Set CHx polarity to active high
    timer->CCER |= CCER_CCxE; // Enable TIMx Channel x
}

void PWM_SetPos(TIM_TypeDef *timer, uint8_t channel, uint8_t angle)
{
    
    if (channel < 1 || channel > 4)
        return;
    if (angle > 180) 
        angle = 180;

    uint16_t pulse = 1000 + ((uint16_t)angle*1000/180);

    if (channel == 1)
        timer->CCR1 = pulse;
    else if (channel == 2)
        timer->CCR2 = pulse;
    else if (channel == 3)
        timer->CCR3 = pulse;
    else if (channel == 4)
        timer->CCR4 = pulse;
}