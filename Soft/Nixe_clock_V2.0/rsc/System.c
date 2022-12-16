#include "System.h"

void initClock() {
  CLK->CKDIVR &= ~(CLK_CKDIVR_HSIDIV | CLK_CKDIVR_CPUDIV);
  CLK->PCKENR1 = 0xff;
  CLK->PCKENR2 = 0xff;
}

void initGPIO() {
  GPIOA->DDR |= (1 << 3);
  
  GPIOC->DDR |= (7 << 3);
  GPIOC->CR1 |= (7 << 3);
  GPIOC->CR2 |= (7 << 3);
  
  GPIOD->DDR |= (7 << 4);
  GPIOD->CR1 |= (7 << 4);
  GPIOD->CR2 |= (7 << 4);
  
  GPIOA->DDR &= ~(1 << 1);
  GPIOA->CR1 |= (1 << 1);
  GPIOA->CR2 |= (1 << 1);
  
  EXTI->CR1 |= EXTI_CR1_PAIS;
}

void initTimers() {
  TIM1->PSCRH = 0;
  TIM1->PSCRL = 15;
  TIM1->ARRH = 0;
  TIM1->ARRL = 55;
  TIM1->BKR |= TIM1_BKR_MOE;
  TIM1->CCMR1 |= TIM1_CCMR_OCxPE;
  TIM1->CR1 |= TIM1_CR1_CEN;
  
  TIM1->CCMR1 |= 0x60;
  TIM1->CCER1 |= TIM1_CCER1_CC1E;
  TIM1->CCR1H = 0;
  TIM1->CCR1L = 45;
  
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