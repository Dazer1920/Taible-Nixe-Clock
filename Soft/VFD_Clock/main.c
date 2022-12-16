#include "System.h"
#include "Lamp.h"
#include "Buttons.h"
#include "Clock.h"

int main() {
  initClock();
  initGPIO();
  initTimers();
  
  asm("RIM");
  
  delay(500);
  GPIOA->ODR |= (1 << 3);
  
  initLamp();
  initButtons();
  initI2C(F_CPU, 100000);
  initDs1307();
  
  uint8_t l = 0;
    
  while(1) {
    if(TIM4->SR1 & TIM4_SR1_UIF) {
      TIM4->SR1 &= ~TIM4_SR1_UIF;
      
      processButtons();
      processClock();
      processDisplay();
    }
  }
}