#include "IMU.h"
#include "Comms.h"
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
  GPIOA->AFR[0] |=  (0x5U << GPIO_AFRL_AFSEL5_Pos) |
                     (0x5U << GPIO_AFRL_AFSEL6_Pos) |
                     (0x5U << GPIO_AFRL_AFSEL7_Pos);

  GPIOA->OTYPER  &= ~(GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);
  GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);
  GPIOA->OSPEEDR |=  (GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7);

  // CS: PA4 as push-pull output, idle HIGH (deselected)
  GPIOA->MODER &= ~GPIO_MODER_MODE4;
  GPIOA->MODER |=  GPIO_MODER_MODE4_0;      
  GPIOA->OTYPER &= ~GPIO_OTYPER_OT4;  // push-pull
  GPIOA->PUPDR  &= ~GPIO_PUPDR_PUPD4;
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED4;
  IMU_CS_High(); // Start deselected

  Comms_Init(IMU);

  // Low Noise mode for gyroscope and accelerometer
  IMU_WriteReg(0x10, 0x0F);
  IMU_WriteReg(0x1B, 0x16); // +/-16g @ 800Hz for accel
  IMU_WriteReg(0x1C, 0x36); // +/-500dps @ 800Hz for gyro
}

__IMU_RESP IMU_GetWhoAmI(void){
  __IMU_RESP response;
  IMU_CS_Low();
  response.rx0 = IMU_SPI1_Transfer(0xF2);
  response.rx1 = IMU_SPI1_Transfer(0x00);
  IMU_CS_High();
  return response;
}

uint8_t IMU_SPI1_Transfer(uint8_t data){
  while (!(SPI1->SR & SPI_SR_TXP)); // Wait for TXP to be available for TX ops
  *(__IO uint8_t *)&SPI1->TXDR = data;
  while (!(SPI1->SR & SPI_SR_RXP)); // Wait for RXP to be available for RX ops
  return *(__IO uint8_t *)&SPI1->RXDR;
}

uint8_t IMU_ReadReg(uint8_t reg){
  IMU_CS_Low();
  IMU_SPI1_Transfer(reg | 0x80); // Read specified register in read mode
  uint8_t val = IMU_SPI1_Transfer(0x00);
  IMU_CS_High();
  return val;
}

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

void IMU_ReadTemperature(float *tmp){
  uint8_t buf[2];
  IMU_CS_Low();
  IMU_SPI1_Transfer(0x0C | 0x80);
  for (int i=0; i < 2; i++){
    buf[i] = IMU_SPI1_Transfer(0x00);
  }
  IMU_CS_High();
  float temp_raw = (float)(((uint16_t)buf[1] << 8) | buf[0]);
  *tmp = (float)((temp_raw / 128) + 25);
}

void IMU_CS_High(void){
  GPIOA->BSRR = GPIO_BSRR_BS4; // Set CS Pin
}

void IMU_CS_Low(void){
  GPIOA->BSRR = GPIO_BSRR_BR4; // Reset CS Pin
}