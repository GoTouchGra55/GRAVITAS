#include "Comms.h"
#include "stm32h743xx.h"

void Comms_Init(SENSOR sensor){
  SPI_TypeDef *SPI_PORTS[] = {SPI1, SPI2};
  int size = sizeof(SPI_PORTS)/sizeof(SPI_PORTS[0]);
  if (sensor > size) 
    return;

  SPI_TypeDef *SPI = SPI_PORTS[sensor];
  // Disable SPI before configuration
  SPI->CR1 &= ~SPI_CR1_SPE;
  // 8-bit data size
  SPI->CFG1 &= ~SPI_CFG1_DSIZE;
  SPI->CFG1 |= (7U << SPI_CFG1_DSIZE_Pos);
  // FIFO threshold = 1 data frame
  SPI->CFG1 &= ~SPI_CFG1_FTHLV;
  // Slow clock
  SPI->CFG1 &= ~SPI_CFG1_MBR;
  SPI->CFG1 |= (3U << SPI_CFG1_MBR_Pos);
  // Master
  SPI->CFG2 |= SPI_CFG2_MASTER;
  // Full duplex
  SPI->CFG2 &= ~SPI_CFG2_COMM;
  // Mode 0
  SPI->CFG2 &= ~SPI_CFG2_CPOL;
  SPI->CFG2 &= ~SPI_CFG2_CPHA;
  // Software NSS management
  SPI->CFG2 |= SPI_CFG2_SSM;
  SPI->CR1  |= SPI_CR1_SSI;   // prevents MODF
  // Enable
  SPI->CR1 |= SPI_CR1_SPE;
  // Start transfer
  SPI->CR1 |= SPI_CR1_CSTART;
}