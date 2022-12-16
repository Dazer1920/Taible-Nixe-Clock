#include "Clock.h"

#define STATE_LAMP_IN_MENU ON

extern uint8_t ValueLamp[NUMBER_LAMPS];
extern Enable DotEnable;
extern Enable digisEnable[NUMBER_LAMPS];

extern Enable ButtonTrigger[NUMBER_BUTTONS];

Enable isMenu = OFF;
uint8_t countPosit = 0;

uint8_t seconds = 0, minute = 0, hour = 0;

void initDs1307() {
  uint8_t vals[8];
  Enable init = OFF;
  ReadI2C(0xd0, 0, vals, 8);
  
  if(vals[0] & (1 << 7)) {
    vals[0] &= ~(1 << 7);
    init = ON;
  }
  
  if(vals[2] & (1 << 6)) {
    vals[2] &= ~(1 << 6);
    init = ON;
  }
    
  if(!(vals[7] & (1 << 7)) || !(vals[7] & (1 << 4)) || (vals[7] & (1 << 0)) || (vals[7] & (1 << 1))) {
    vals[7] = 0;
    vals[7] |= (1 << 4) | (1 << 7);
    init = ON;
  }
  
  if(init) WriteI2C(0xd0, 0, vals, 8);
}

uint16_t delCountBlink = 0, countDelPressed, countDelInc;
Enable triggerBLink = OFF;

void incrementTime() {
  if(countPosit == 1) {
    hour++;
    if(hour > 23) hour = 0;
  } else 
  if(countPosit == 2) {
    minute++; 
    if(minute > 59) minute = 0;
  } else 
  if(countPosit == 3) {
    seconds++; 
    if(seconds > 59) seconds = 0;
  }
}

void decrementTime() {
  if(countPosit == 1) {
    if(hour == 0) hour = 24;
    hour--; 
  } else 
  if(countPosit == 2) {
    if(minute == 0) minute = 60;
    minute--; 
  } else 
  if(countPosit == 3) {
    if(seconds == 0) seconds = 60;
    seconds--; 
  }
}

void onClickButton(uint8_t key) {
  if(key == OK_BUTTON) {
    if(isMenu == OFF) isMenu = ON;
    if(isMenu == ON && ++countPosit > 3) {
      isMenu = OFF;
      delCountBlink = 0;
      countPosit = 0;
      
      digisEnable[0] = ON;
      digisEnable[1] = ON;
      digisEnable[2] = ON;
      digisEnable[3] = ON;
      digisEnable[4] = ON;
      digisEnable[5] = ON;
      
      uint8_t vals[3];
      vals[0] = decimalToBcd(seconds);      
      vals[1] = decimalToBcd(minute);      
      vals[2] = decimalToBcd(hour);
      
      WriteI2C(0xd0, 0, vals, 3);
    }
  }
  
  if(key == UP_BUTTON) {
    delCountBlink = 0;
    triggerBLink = ON;
    incrementTime();
  }
  
  if(key == DOWN_BUTTON) {
    delCountBlink = 0;
    triggerBLink = ON;
    decrementTime();
  }
}

void processMenu() {
  DotEnable = OFF;
  
  if(ButtonTrigger[DOWN_BUTTON] == OFF && ButtonTrigger[UP_BUTTON] == OFF && ++delCountBlink >= 200) {
    delCountBlink = 0;
    triggerBLink = (triggerBLink == ON)? OFF : ON;
  }
  
  if(countPosit == 1) {
    digisEnable[0] = triggerBLink;
    digisEnable[1] = triggerBLink;
    digisEnable[2] = STATE_LAMP_IN_MENU;
    digisEnable[3] = STATE_LAMP_IN_MENU;
    digisEnable[4] = STATE_LAMP_IN_MENU;
    digisEnable[5] = STATE_LAMP_IN_MENU;
  } else 
  if(countPosit == 2) {
    digisEnable[0] = STATE_LAMP_IN_MENU;
    digisEnable[1] = STATE_LAMP_IN_MENU;
    digisEnable[2] = triggerBLink;
    digisEnable[3] = triggerBLink;
    digisEnable[4] = STATE_LAMP_IN_MENU;
    digisEnable[5] = STATE_LAMP_IN_MENU;
  } else 
  if(countPosit == 3) {
    digisEnable[0] = STATE_LAMP_IN_MENU;
    digisEnable[1] = STATE_LAMP_IN_MENU;
    digisEnable[2] = STATE_LAMP_IN_MENU;
    digisEnable[3] = STATE_LAMP_IN_MENU;
    digisEnable[4] = triggerBLink;
    digisEnable[5] = triggerBLink;
  }
  
  if(ButtonTrigger[DOWN_BUTTON] == ON || ButtonTrigger[UP_BUTTON] == ON) {
    if(countDelPressed > 300) {
      if(++countDelInc >= 100) {
        countDelInc = 0;
        
        if(ButtonTrigger[UP_BUTTON] == ON) {
          incrementTime();
        } else 
        if(ButtonTrigger[DOWN_BUTTON] == ON) {
          decrementTime();
        }
      }
    } countDelPressed++;
  } else countDelPressed = 0;
}

Enable isReadClock = OFF;
#pragma vector = 7
__interrupt void EXTI3(){
  if(isMenu == ON) return;
  
  if(GPIOC->IDR & (1 << 3)) {
    DotEnable = ON;
    isReadClock = ON;
  } else {
    DotEnable = OFF;
  }
}

uint8_t countDelDs = 0;
uint8_t oldSeconds = 0;
uint8_t vals[3];
void processClock() {
  if(isMenu == ON) {
    processMenu();
  } else {
    if(isReadClock) {
      isReadClock = OFF;
      
      ReadI2C(0xd0, 0, vals, 3);
      seconds = bcdToDecimal(vals[0]);
      minute = bcdToDecimal(vals[1]);
      hour = bcdToDecimal(vals[2]);
    }
  }
  
  ValueLamp[0] = hour / 10;
  ValueLamp[1] = hour % 10;
    
  ValueLamp[2] = minute / 10;
  ValueLamp[3] = minute % 10;
        
  ValueLamp[4] = seconds / 10;
  ValueLamp[5] = seconds % 10;
}