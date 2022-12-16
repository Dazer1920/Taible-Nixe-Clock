#include "Lamp.h"

Enable PointEnable[NUMBER_LAMPS], digisEnable[NUMBER_LAMPS];
Enable DotEnable = OFF;
uint8_t ValueLamp[NUMBER_LAMPS];

#define GRID1_POSIT ((uint16_t)(1 << 6))
#define GRID2_POSIT ((uint16_t)(1 << 7))
#define GRID3_POSIT ((uint16_t)(1 << 5))
#define GRID4_POSIT ((uint16_t)(1 << 4))
#define GRID5_POSIT ((uint16_t)(1 << 3))
#define GRID6_POSIT ((uint16_t)(1 << 2))

#define POINT_POSIT ((uint16_t) (1 << 9))

#define A_POSIT ((uint16_t)(1 << 14))
#define B_POSIT ((uint16_t)(1 << 12))
#define C_POSIT ((uint16_t)(1 << 1))
#define D_POSIT ((uint16_t)(1 << 10))
#define E_POSIT ((uint16_t)(1 << 11))
#define F_POSIT ((uint16_t)(1 << 13))
#define G_POSIT ((uint16_t)(1 << 15))

void initLamp() {
  for(uint8_t i = 0; i < NUMBER_LAMPS; i++) digisEnable[i] = ON;
}

void softSpiWrite(uint16_t data) {
  for(uint8_t i = 0; i < 16; i++) {
    if((data << i) & 0x8000) GPIOD->ODR |= (1 << 4);
    else GPIOD->ODR &= ~(1 << 4);
    
    GPIOD->ODR |= (1 << 6);
    GPIOD->ODR &= ~(1 << 6);
  }
  
  GPIOD->ODR |= (1 << 5);
  GPIOD->ODR &= ~(1 << 5);
}

uint16_t digisLampSymbols[] = {
  A_POSIT | B_POSIT | C_POSIT | D_POSIT | E_POSIT | F_POSIT,            // 0
  B_POSIT | C_POSIT,                                                    // 1
  A_POSIT | B_POSIT | D_POSIT | E_POSIT | G_POSIT,                      // 2
  A_POSIT | B_POSIT | C_POSIT | D_POSIT | G_POSIT,                      // 3
  B_POSIT | C_POSIT | F_POSIT | G_POSIT,                                // 4
  A_POSIT | C_POSIT | D_POSIT | F_POSIT | G_POSIT,                      // 5
  A_POSIT | C_POSIT | D_POSIT | E_POSIT | F_POSIT | G_POSIT,            // 6
  A_POSIT | B_POSIT | C_POSIT,                                          // 7
  A_POSIT | B_POSIT | C_POSIT | D_POSIT | E_POSIT | F_POSIT | G_POSIT,  // 8
  A_POSIT | B_POSIT | C_POSIT | D_POSIT | F_POSIT | G_POSIT,            // 9
};

uint16_t lampControlGridPosit[] = {GRID1_POSIT, GRID2_POSIT, GRID3_POSIT, GRID4_POSIT, GRID5_POSIT, GRID6_POSIT};

uint8_t countLamp = 0;
void processDisplay() {
  countLamp++;
  if(countLamp >= NUMBER_LAMPS) countLamp = 0;
  
  uint16_t outByte = 0;
  softSpiWrite(~outByte);
  
  outByte |= digisLampSymbols[ValueLamp[countLamp]];
  if(digisEnable[countLamp] == ON) outByte |= lampControlGridPosit[countLamp];
  
  if(countLamp == 1 || countLamp == 3) {
    if(DotEnable == ON) outByte |= POINT_POSIT;
  }
  
  softSpiWrite(~outByte);
}