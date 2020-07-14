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
    processDisplay();
    processButtons();
    processClock();
    
    delay(1);
  }
}
