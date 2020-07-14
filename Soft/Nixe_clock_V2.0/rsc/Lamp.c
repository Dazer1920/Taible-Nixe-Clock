#include "Lamp.h"

Enable PointEnable[NUMBER_LAMPS], digisEnable[NUMBER_LAMPS];
Enable DotEnable = OFF;
uint8_t ValueLamp[NUMBER_LAMPS];

void initLamp() {
  for(uint8_t i = 0; i < NUMBER_LAMPS; i++) digisEnable[i] = ON;
}

void softSpiWrite(uint8_t data) {
  for(uint8_t i = 0; i < 8; i++) {
    GPIOD->ODR = ((data << i) & 0x80)? GPIOD->ODR | (1 << 4) : GPIOD->ODR & ~(1 << 4);
    
    GPIOD->ODR |= (1 << 6);
    GPIOD->ODR &= ~(1 << 6);
  }
  
  GPIOD->ODR |= (1 << 5);
  GPIOD->ODR &= ~(1 << 5);
}

void writeDigis(uint8_t data) {
  GPIOC->ODR = (data & 0x01)? GPIOC->ODR | (1 << 3) : GPIOC->ODR & ~(1 << 3);
  GPIOC->ODR = (data & 0x02)? GPIOC->ODR | (1 << 5) : GPIOC->ODR & ~(1 << 5);
  GPIOA->ODR = (data & 0x04)? GPIOA->ODR | (1 << 3) : GPIOA->ODR & ~(1 << 3);
  GPIOC->ODR = (data & 0x08)? GPIOC->ODR | (1 << 4) : GPIOC->ODR & ~(1 << 4);
}

uint8_t lampAnodePosit[] = {1 << 1, 1 << 2, 1 << 4, 1 << 5, 1 << 6, 1 << 7};
uint8_t countLamp = 0;
void processDisplay() {
  countLamp++;
  if(countLamp > NUMBER_LAMPS) countLamp = 0;
  
  uint8_t outByte = 0;
  softSpiWrite(outByte);
  
  if(countLamp <= 5) {
    writeDigis(ValueLamp[countLamp]);
    if(digisEnable[countLamp] == ON) outByte |= lampAnodePosit[countLamp];
    if(digisEnable[countLamp] == ON && PointEnable[countLamp] == ON) outByte |= 1;
  } else {
    if(DotEnable == ON) outByte = 1 << 3;
  }
  
  softSpiWrite(outByte);
}