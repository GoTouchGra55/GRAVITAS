#include "IMU.h"
#include "main.h"
#include "stdio.h"
#include "stm32h743xx.h"
#include "usbd_cdc_if.h"

void IMU_Init(void){
  // Enable clock
  RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;

  // SPI1: PA5=SCK, PA6=MISO, PA7=MOSI 
  GPIOA->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
  GPIOA->MODER |=  (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);

  GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL5 | GPIO_AFRL_AFSEL6 | GPIO_AFRL_AFSEL7);
  GPIOA->AFR[0] |=  (5U << GPIO_AFRL_AFSEL5_Pos) |
                     (5U << GPIO_AFRL_AFSEL6_Pos) |
                     (5U << GPIO_AFRL_AFSEL7_Pos);

  GPIOA->OTYPER  &= ~(GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);
  GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);
  GPIOA->OSPEEDR |=  (GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7);

  // CS: PA4 as push-pull output, idle HIGH (deselected)
  GPIOA->MODER &= ~GPIO_MODER_MODE4;
  GPIOA->MODER |=  GPIO_MODER_MODE4_0;      
  GPIOA->OTYPER &= ~GPIO_OTYPER_OT4;  // push-pull
  GPIOA->PUPDR  &= ~GPIO_PUPDR_PUPD4;
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED4;
  GPIOA->BSRR = GPIO_BSRR_BS4;  // start deselected
}

__IMU_RESP IMU_GetWhoAmI(void){
  __IMU_RESP response;
  HAL_Delay(100);
  IMU_CS_Low();
  response.rx0 = IMU_SPI1_Transfer(0xF2);
  response.rx1 = IMU_SPI1_Transfer(0x00);
  IMU_CS_High();
  return response;
}

void IMU_Comms_Init(void)
{
  // Disable SPI before configuration
  SPI1->CR1 &= ~SPI_CR1_SPE;
  // 8-bit data size
  SPI1->CFG1 &= ~SPI_CFG1_DSIZE;
  SPI1->CFG1 |= (7U << SPI_CFG1_DSIZE_Pos);
  // FIFO threshold = 1 data frame
  SPI1->CFG1 &= ~SPI_CFG1_FTHLV;
  // Slow clock
  SPI1->CFG1 &= ~SPI_CFG1_MBR;
  SPI1->CFG1 |= (3U << SPI_CFG1_MBR_Pos);
  // Master
  SPI1->CFG2 |= SPI_CFG2_MASTER;
  // Full duplex
  SPI1->CFG2 &= ~SPI_CFG2_COMM;
  // Mode 0
  SPI1->CFG2 &= ~SPI_CFG2_CPOL;
  SPI1->CFG2 &= ~SPI_CFG2_CPHA;
  // Software NSS management — we control CS ourselves via GPIO
  SPI1->CFG2 |= SPI_CFG2_SSM;
  SPI1->CR1  |= SPI_CR1_SSI;   // prevents MODF, tells peripheral "NSS is high" internally
  // Enable
  SPI1->CR1 |= SPI_CR1_SPE;
  // Start transfer
  SPI1->CR1 |= SPI_CR1_CSTART;
}

uint8_t IMU_SPI1_Transfer(uint8_t data){
  while (!(SPI1->SR & SPI_SR_TXP));
  *(__IO uint8_t *)&SPI1->TXDR = data;
  while (!(SPI1->SR & SPI_SR_RXP));
  return *(__IO uint8_t *)&SPI1->RXDR;
}

// uint8_t IMU_ReadReg(uint8_t reg);
// void IMU_ReadRegs(uint8_t reg, uint8_t *buf, uint8_t len);
// void IMU_WriteReg(uint8_t reg, uint8_t data);

// void IMU_ReadAccel(int16_t *ax, int16_t *ay, int16_t *az);
// void IMU_ReadGyro(int16_t *gx, int16_t *gy, int16_t *gz);
// int16_t IMU_ReadTemperature(void);

void IMU_CS_High(void){
  GPIOA->BSRR |= GPIO_BSRR_BS4;
}

void IMU_CS_Low(void){
  GPIOA->BSRR |= GPIO_BSRR_BR4;
}