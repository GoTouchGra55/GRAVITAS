#ifndef IMU_H
#define IMU_H

#include "stm32h743xx.h"

typedef struct{
  uint8_t rx0;
  uint8_t rx1;
} __IMU_RESP;

void IMU_Init(void);
void IMU_Comms_Init(void);
__IMU_RESP IMU_GetWhoAmI(void);
uint8_t IMU_SPI1_Transfer(uint8_t data);

void IMU_WriteReg(uint8_t reg, uint8_t data);
uint8_t IMU_ReadReg(uint8_t reg);

void IMU_ReadAccelRAW(int16_t *ax, int16_t *ay, int16_t *az);
void IMU_ReadAccel(float *ax, float *ay, float *az);
void IMU_ReadGyroRAW(int16_t *gx, int16_t *gy, int16_t *gz);
void IMU_ReadGyro(float *gx, float *gy, float *gz);
void IMU_ReadTemperature(float *tmp);

void IMU_CS_High(void);
void IMU_CS_Low(void);

#endif