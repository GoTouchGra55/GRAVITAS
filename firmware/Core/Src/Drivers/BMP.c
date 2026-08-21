#include "BMP.h"
#include "Comms.h"
#include "math.h"
#include <stm32h743xx.h>

void BMP_Init(BMP_PWR_MODE Power_mode, uint8_t OSR, uint8_t ODR){
  uint8_t osr_config = (0x01 << 6) |       // press_en 
                        (OSR << 3) |       // OSR_P = set by user 
                        (0x00 << 0);       // OSR_T = 1x

  uint8_t odr_config = (ODR << 2) | Power_mode;  

  // Enable clocks
  RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN | RCC_AHB4ENR_GPIOCEN;
  
  // SPI2: PB13=SCK, PC1=MOSI, PC2=MISO
  GPIOB->MODER &= ~GPIO_MODER_MODE13;
  GPIOB->MODER |= GPIO_MODER_MODE13_1;

  GPIOC->MODER &= ~(GPIO_MODER_MODE1 | GPIO_MODER_MODE2);
  GPIOC->MODER |= (GPIO_MODER_MODE1_1 | GPIO_MODER_MODE2_1);

  GPIOB->AFR[1] &= ~GPIO_AFRH_AFSEL13;
  GPIOB->AFR[1] |= (0x5U << GPIO_AFRH_AFSEL13_Pos);

  GPIOC->AFR[0] &= ~(GPIO_AFRL_AFSEL1 | GPIO_AFRL_AFSEL2);
  GPIOC->AFR[0] |= (0x5U << GPIO_AFRL_AFSEL1_Pos) |
                    (0x5U << GPIO_AFRL_AFSEL2_Pos);

  GPIOC->MODER &= ~GPIO_MODER_MODE0;
  GPIOC->MODER |= GPIO_MODER_MODE0_0;
  GPIOC->OTYPER &= ~GPIO_OTYPER_OT0;
  GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD0;
  GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED0;

  BMP_CS_High(); // Start deselected
  Comms_Init(BMP);

  // DO NOT MOVE AHEAD OF COMMS_INIT! >:(
  // Select NORMAL mode and ODR
  BMP_WriteReg(0x37, odr_config);
  // Set OSR
  BMP_WriteReg(0x36, osr_config);
}

void BMP_WriteReg(uint8_t reg, uint8_t data){
  BMP_CS_Low();
  BMP_SPI2_Transfer(reg & 0x7F);
  BMP_SPI2_Transfer(data);
  BMP_CS_High();
}

uint8_t BMP_SPI2_Transfer(uint8_t data){
  while (!(SPI2->SR & SPI_SR_TXP)); // Wait for TXP to be available for TX ops
  *(__IO uint8_t *)&SPI2->TXDR = data;
  while (!(SPI2->SR & SPI_SR_RXP)); // Wait for RXP to be available for RX ops
  return *(__IO uint8_t *)&SPI2->RXDR;
}

uint8_t BMP_GetWhoami(void){
  // CHIP_ID
  BMP_CS_Low();
  BMP_SPI2_Transfer(0x81);
  uint8_t id = BMP_SPI2_Transfer(0x00);
  BMP_CS_High();
  return id;
}

float BMP_ReadPressureRaw(void){
  uint8_t buf[3];
  BMP_CS_Low();
  BMP_SPI2_Transfer(0x20 | 0x80);
  buf[0] = BMP_SPI2_Transfer(0x00);
  buf[1] = BMP_SPI2_Transfer(0x00);
  buf[2] = BMP_SPI2_Transfer(0x00);
  BMP_CS_High();
  uint32_t raw_pressure =
    ((uint32_t)buf[2] << 16) |
    ((uint32_t)buf[1] << 8)  |
    buf[0];
  return raw_pressure / 64.0f;
}

float BMP_ReadHeight(float ground_pressure){
    float pressure = BMP_ReadPressureRaw();
    float ratio = powf(
        pressure / ground_pressure,
        1.0f / 5.255f
    );
    return 44330.0f * (1.0f - ratio);
}

void BMP_CS_High(){
  GPIOC->BSRR = GPIO_BSRR_BS0;
}

void BMP_CS_Low(){
  GPIOC->BSRR = GPIO_BSRR_BR0;
}