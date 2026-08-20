#include "BMP.h"
#include "Comms.h"
#include <stm32h743xx.h>

void BMP_Init(void){
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
}

uint8_t BMP_SPI2_Transfer(uint8_t data){
  while (!(SPI2->SR & SPI_SR_TXP)); // Wait for TXP to be available for TX ops
  *(__IO uint8_t *)&SPI2->TXDR = data;
  while (!(SPI2->SR & SPI_SR_RXP)); // Wait for RXP to be available for RX ops
  return *(__IO uint8_t *)&SPI2->RXDR;
}

void BMP_CS_High(){
  GPIOC->BSRR = GPIO_BSRR_BS0;
}

void BMP_CS_Low(){
  GPIOC->BSRR = GPIO_BSRR_BR0;
}