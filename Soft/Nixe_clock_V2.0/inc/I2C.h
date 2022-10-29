#ifndef _I2C_H
#define _I2C_H

#include "System.h"

typedef enum {
    I2C_SUCCESS = 0,
    I2C_TIMEOUT,
    I2C_ERROR
} t_i2c_status;
  
void initI2C(uint32_t f_master_hz, uint32_t f_i2c_hz);                       
void WriteI2C(uint8_t address, uint8_t reg_addr, uint8_t * data, uint8_t length);
void ReadI2C(uint8_t address, uint8_t reg_addr, uint8_t * data, uint8_t length);

#endif