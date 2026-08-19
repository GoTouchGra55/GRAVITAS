#include "IMU.h"
#include "main.h"
#include "stdio.h"
#include "stm32h743xx.h"
#include "usbd_cdc_if.h"

void IMU_WriteReg(uint8_t reg, uint8_t data){
  IMU_CS_Low();
  IMU_SPI1_Transfer(reg & 0x7F); // Bit 7 = 0 for write
  IMU_SPI1_Transfer(data);
  IMU_CS_High();
}

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

  IMU_Comms_Init();

  // Low Noise mode for gyroscope and accelerometer
  IMU_WriteReg(0x10, 0x0F);
  IMU_WriteReg(0x1B, 0x16);
  IMU_WriteReg(0x1C, 0x36);
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
  // Software NSS management
  SPI1->CFG2 |= SPI_CFG2_SSM;
  SPI1->CR1  |= SPI_CR1_SSI;   // prevents MODF
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

uint8_t IMU_ReadReg(uint8_t reg){
  IMU_CS_Low();
  IMU_SPI1_Transfer(reg | 0x80);
  uint8_t val = IMU_SPI1_Transfer(0x00);
  IMU_CS_High();
  return val;
}
// void IMU_ReadRegs(uint8_t reg, uint8_t *buf, uint8_t len);

void IMU_ReadAccelRAW(int16_t *ax, int16_t *ay, int16_t *az){

  uint8_t buf[6];
  IMU_CS_Low();
  IMU_SPI1_Transfer(0x80);
  for (int i=0; i < 6; i++){
    buf[i] = IMU_SPI1_Transfer(0x00);
  }
  IMU_CS_High();

  *ax = (int16_t)((buf[1] << 8) | buf[0]);
  *ay = (int16_t)((buf[3] << 8) | buf[2]);
  *az = (int16_t)((buf[5] << 8) | buf[4]);
}
void IMU_ReadGyroRAW(int16_t *gx, int16_t *gy, int16_t *gz){

  uint8_t buf[6];
  IMU_CS_Low();
  IMU_SPI1_Transfer(0x86);
  for (int i=0; i < 6; i++){
    buf[i] = IMU_SPI1_Transfer(0x00);
  }
  IMU_CS_High();

  *gx = (int16_t)(((uint16_t)buf[1] << 8) | buf[0]);
  *gy = (int16_t)(((uint16_t)buf[3] << 8) | buf[2]);
  *gz = (int16_t)(((uint16_t)buf[5] << 8) | buf[4]);
}

void IMU_ReadAccel(float *ax, float *ay, float *az)
{
  int16_t raw_ax, raw_ay, raw_az;
  IMU_ReadAccelRAW(&raw_ax, &raw_ay, &raw_az);
  *ax = (float)raw_ax / 2048.0f;
  *ay = (float)raw_ay / 2048.0f;
  *az = (float)raw_az / 2048.0f;
}

void IMU_ReadGyro(float *gx, float *gy, float *gz)
{
  int16_t raw_gx, raw_gy, raw_gz;
  IMU_ReadGyroRAW(&raw_gx, &raw_gy, &raw_gz);
  *gx = (float)raw_gx / 65.5f;
  *gy = (float)raw_gy / 65.5f;
  *gz = (float)raw_gz / 65.5f;
}

// int16_t IMU_ReadTemperature(void);

void IMU_CS_High(void){
  GPIOA->BSRR |= GPIO_BSRR_BS4;
}

void IMU_CS_Low(void){
  GPIOA->BSRR |= GPIO_BSRR_BR4;
}