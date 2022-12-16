#include "System.h"

void initClock() {
  CLK->CKDIVR &= ~(CLK_CKDIVR_HSIDIV | CLK_CKDIVR_CPUDIV);
  CLK->PCKENR1 = 0xff;
  CLK->PCKENR2 = 0xff;
}

void initGPIO() {
  GPIOA->DDR |= (1 << 3);
  GPIOA->CR1 |= (1 << 3);
  GPIOA->CR2 |= (1 << 3);
  
  GPIOC->DDR &= ~(1 << 4);
  GPIOC->CR1 &= ~(1 << 4);
  GPIOC->CR2 &= ~(1 << 4);
  
  GPIOD->DDR |= (7 << 4);
  GPIOD->CR1 |= (7 << 4);
  GPIOD->CR2 |= (7 << 4);
  
  GPIOC->DDR &= ~(1 << 3);
  GPIOC->CR1 |= (1 << 3);
  GPIOC->CR2 |= (1 << 3);
  
  EXTI->CR1 |= EXTI_CR1_PCIS;
}

void initTimers() {
  TIM4->PSCR = 0x07;
  TIM4->CR1 |= TIM4_CR1_URS | TIM4_CR1_CEN;
  TIM4->ARR = 125;
}

uint8_t bcdToDecimal(uint8_t value){
    return ((value & 0x0F) + (((value & 0xF0) >> 0x04) * 0x0A));
}

uint8_t decimalToBcd(uint8_t value){
    return (((value / 0x0A) << 0x04) & 0xF0) | ((value % 0x0A) & 0x0F);
}

uint32_t tickDelay = 0;
void delay(volatile uint32_t ms) {
  tickDelay = 0;
  TIM4->CNTR = 0;
  while(1) {
    if(TIM4->SR1 & TIM4_SR1_UIF) {
      TIM4->SR1 &= ~TIM4_SR1_UIF;
      if(tickDelay == ms) return;
      tickDelay++;
    }
  }
}