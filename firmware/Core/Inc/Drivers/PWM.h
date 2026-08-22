#ifndef GRAVITAS_PWM_H
#define GRAVITAS_PWM_H

#include "main.h"
#include <stdint.h>
#include <stm32h743xx.h>

void PWM_Init(void);
void PWM_SetPos(TIM_TypeDef *timer, uint8_t channel, uint8_t angle);
void PWM_FakePID(TIM_TypeDef *timer, GYRO_DATA *gyro_data, ACC_DATA *acc_data);

#endif