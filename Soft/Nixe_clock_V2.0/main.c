#include "System.h"
#include "Lamp.h"
#include "Buttons.h"
#include "Clock.h"

int main() {
  initClock();
  initGPIO();
  initTimers();
  initLamp();
  initButtons();
  initI2C(F_CPU, 100000);
  initDs1307();
  
  while(1) {
    if(TIM4->SR1 & TIM4_SR1_UIF) {
      TIM4->SR1 &= ~TIM4_SR1_UIF;
      
      processDisplay();
      processButtons();
      processClock();
    }
  }
}
