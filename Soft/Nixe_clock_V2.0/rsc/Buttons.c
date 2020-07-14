#include "Buttons.h"

GPIO_TypeDef* ButtonsPort[NUMBER_BUTTONS];
uint8_t ButtonPins[NUMBER_BUTTONS], delCountButtOn[NUMBER_BUTTONS], delCountButtOff[NUMBER_BUTTONS];
Enable ButtonTrigger[NUMBER_BUTTONS];

void initButtons() {
  GPIOD->DDR &= ~(3 << 2);
  GPIOD->CR1 |= (3 << 2);
  GPIOD->CR2 &= ~(3 << 2);

  GPIOC->DDR &= ~(1 << 7);
  GPIOC->CR1 |= (1 << 7);
  GPIOC->CR2 &= ~(1 << 7);
  
  ButtonsPort[UP_BUTTON] = GPIOD;
  ButtonsPort[OK_BUTTON] = GPIOD;
  ButtonsPort[DOWN_BUTTON] = GPIOC;
  
  ButtonPins[UP_BUTTON] = 3;
  ButtonPins[OK_BUTTON] = 2;
  ButtonPins[DOWN_BUTTON] = 7;
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