#include "Buttons.h"

GPIO_TypeDef* ButtonsPort[NUMBER_BUTTONS];
uint8_t ButtonPins[NUMBER_BUTTONS], delCountButtOn[NUMBER_BUTTONS], delCountButtOff[NUMBER_BUTTONS];
Enable ButtonTrigger[NUMBER_BUTTONS];

void initButtons() {
  GPIOC->DDR &= ~(7 << 5);
  GPIOC->CR1 |= (7 << 5);
  GPIOC->CR2 &= ~(7 << 5);
  
  ButtonsPort[UP_BUTTON] = GPIOC;
  ButtonsPort[OK_BUTTON] = GPIOC;
  ButtonsPort[DOWN_BUTTON] = GPIOC;
  
  ButtonPins[UP_BUTTON] = 7;
  ButtonPins[OK_BUTTON] = 6;
  ButtonPins[DOWN_BUTTON] = 5;
}

__weak void onClickButton(uint8_t key) { }
__weak void unPressedButton(uint8_t key) { }

uint8_t counterButtons = 0;

void processButtons() {
  counterButtons++;
  if(counterButtons >= NUMBER_BUTTONS) counterButtons = 0;
  
  if(ButtonsPort[counterButtons]->IDR & (1 << ButtonPins[counterButtons])) {
    if(ButtonTrigger[counterButtons] == ON && delCountButtOff[counterButtons]++ >= 15) {
      delCountButtOn[counterButtons] = 0;
      delCountButtOff[counterButtons] = 0;
      ButtonTrigger[counterButtons] = OFF;
      
      unPressedButton(counterButtons);
    }
  } else {
    if(ButtonTrigger[counterButtons] == OFF && delCountButtOn[counterButtons]++ >= 15) {
      delCountButtOn[counterButtons] = 0;
      delCountButtOff[counterButtons] = 0;
      ButtonTrigger[counterButtons] = ON;
      
      onClickButton(counterButtons);
    }
  }
}