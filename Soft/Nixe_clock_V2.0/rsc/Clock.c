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
  ReadI2C(0xd0, 0, &seconds);
  if(seconds & (1 << 7)) {
    seconds &= ~(1 << 7);
    WriteI2C(0xd0, 0, &seconds, 1);
  }
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
      
      uint8_t val = decimalToBcd(seconds);
      WriteI2C(0xd0, 0, &val, 1);
      
      val = decimalToBcd(minute);
      WriteI2C(0xd0, 1, &val, 1);
      
      val = decimalToBcd(hour);
      WriteI2C(0xd0, 2, &val, 1);
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

Enable isLoopLamp = OFF;
uint8_t oldMinute, oldHour, countDel, counterDigis, numberLoop;
void LoopLamp() {
  if(oldMinute != minute || oldHour != hour) {
    isLoopLamp = ON;
    if(numberLoop < 3) {
      if(++countDel >= 10) {
        countDel = 0;
        if(++counterDigis >= 10) {
          counterDigis = 0;
          numberLoop++;
        }
      }
    } else {
      numberLoop = 0;
      counterDigis = 0;
      countDel = 0;
      isLoopLamp = OFF;
      
      oldMinute = minute;
      oldHour = hour;
    }
  }
}

void processMenu() {
  DotEnable = OFF;
  
  if(ButtonTrigger[DOWN_BUTTON] == OFF && ButtonTrigger[UP_BUTTON] == OFF && ++delCountBlink >= 100) {
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

uint8_t countDelDs = 0;

void processClock() {
  if(isMenu == ON) {
    processMenu();
  } else {
    if(++delCountBlink >= 250) {
      delCountBlink = 0;
      DotEnable = (DotEnable == ON)? OFF : ON;
    }
    
    if(++countDelDs >= 200) {
      countDelDs = 0;
      
      ReadI2C(0xd0, 0, &seconds);
      ReadI2C(0xd0, 1, &minute);
      ReadI2C(0xd0, 2, &hour);
      
      seconds = bcdToDecimal(seconds);
      minute = bcdToDecimal(minute);
      hour = bcdToDecimal(hour);
    }

    LoopLamp();
  }
  
  if(isLoopLamp == OFF) {
    ValueLamp[0] = hour / 10;
    ValueLamp[1] = hour % 10;
    
    ValueLamp[2] = minute / 10;
    ValueLamp[3] = minute % 10;
        
    ValueLamp[4] = seconds / 10;
    ValueLamp[5] = seconds % 10;
  } else {
    ValueLamp[0] = counterDigis;
    ValueLamp[1] = counterDigis;
    
    ValueLamp[2] = counterDigis;
    ValueLamp[3] = counterDigis;
        
    ValueLamp[4] = counterDigis;
    ValueLamp[5] = counterDigis;
  }
}