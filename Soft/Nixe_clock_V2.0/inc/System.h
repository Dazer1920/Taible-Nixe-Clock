#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "stm8s.h"

#define F_CPU 16000000

typedef enum {
  OFF = 0,
  ON
} Enable;

#define NUMBER_LAMPS 6

void initClock();
void initGPIO();
void initTimers();

void delay(volatile uint32_t ms);

uint8_t bcdToDecimal(uint8_t value);
uint8_t decimalToBcd(uint8_t value);

#endif