#ifndef BMP_H
#define BMP_H

#include <stdint.h>

typedef enum {
  STANDBY = 0x00, 
  NORMAL = 0x01, 
  FORCED = 0x02, 
  NON_STOP = 0x03
} BMP_PWR_MODE;

void BMP_Init(BMP_PWR_MODE Power_mode, uint8_t OSR, uint8_t ODR);
void BMP_CS_High(void);
void BMP_CS_Low(void);
uint8_t BMP_GetWhoami(void);

uint8_t BMP_SPI2_Transfer(uint8_t data);
void BMP_WriteReg(uint8_t reg, uint8_t data);
float BMP_ReadPressureRaw(void);
float BMP_ReadHeight(float GROUND_PRESSURE);

#endif