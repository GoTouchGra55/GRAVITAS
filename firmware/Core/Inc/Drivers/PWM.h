#ifndef GRAVITAS_PWM_H
#define GRAVITAS_PWM_H

#include <stm32h743xx.h>
#include <stdint.h>

void PWM_Init(void);
void PWM_SetPos(TIM_TypeDef *timer, uint8_t channel, uint8_t angle);

#endif