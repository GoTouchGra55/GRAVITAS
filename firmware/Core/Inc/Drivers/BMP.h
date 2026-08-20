#ifndef BMP_H
#define BMP_H

#include <stdint.h>

void BMP_Init(void);
void BMP_CS_High(void);
void BMP_CS_Low(void);

uint8_t BMP_SPI2_Transfer(uint8_t data);

#endif