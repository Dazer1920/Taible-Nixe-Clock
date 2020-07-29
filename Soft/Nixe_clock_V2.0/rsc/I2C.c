#include "I2C.h"
                            
void initI2C(uint32_t f_master_hz, uint32_t f_i2c_hz){
  uint32_t ccr;
   
  GPIOB->DDR &= ~(3 << 4);
  GPIOB->CR1 &= ~(3 << 4);
  GPIOB->CR2 &= ~(3 << 4);
  GPIOB->ODR |= (3 << 4);
   
  I2C->FREQR = 16;
  I2C->CR1 &= ~I2C_CR1_PE;
  I2C->CCRH &= ~I2C_CCRH_FS;
  ccr = f_master_hz / (2 * f_i2c_hz);
  I2C->TRISER = 12 + 1;
  I2C->CCRL = ccr & 0xFF;
  I2C->CCRH = (ccr >> 8) & 0x0F;
  I2C->CR1 |= I2C_CR1_PE;
  I2C->CR2 |= I2C_CR2_ACK;
}

uint16_t tickDelTime = 0;

#define timeOut(ms, flag) \
  tickDelTime = 0; \
  while(1) { \
    if(TIM4->SR1 & TIM4_SR1_UIF) { \
      TIM4->SR1 &= ~TIM4_SR1_UIF; \
      tickDelTime++; \
    } \
    if(tickDelTime == ms) break; \
    if(!flag) break; \
  }

void i2c_start() {
  I2C->CR2 |= I2C_CR2_START;
  timeOut(2, !(I2C->SR1 & I2C_SR1_SB));
}

void i2c_stop() {
  I2C->CR2 |= I2C_CR2_STOP;
  timeOut(2, I2C->SR3 & I2C_SR3_MSL);
}

void i2c_write(uint8_t data) {
  I2C->DR = data;
  timeOut(2, !(I2C->SR1 & I2C_SR1_TXE));
}

uint8_t i2c_read(uint8_t ack) {
  if (ack) I2C->CR2 |= I2C_CR2_ACK;
  else I2C->CR2 &= ~I2C_CR2_ACK;
    
  timeOut(2, !(I2C->SR1 & I2C_SR1_RXNE));
  return I2C->DR;
}

void i2c_write_addr(uint8_t addr) {
  I2C->DR = addr;
  timeOut(2, !(I2C->SR1 & I2C_SR1_ADDR));
  (void) I2C->SR3;
  I2C->CR2 |= I2C_CR2_ACK;
}

void WriteI2C(uint8_t address, uint8_t reg_addr, uint8_t *data, uint8_t length){
  i2c_start();
  i2c_write_addr(address);
  i2c_write(reg_addr);
  while(length--) {
    i2c_write(*data++);
  }
  i2c_stop();
}

void ReadI2C(uint8_t address, uint8_t reg_addr, uint8_t *data){
  i2c_start();
  i2c_write_addr(address);
  i2c_write(reg_addr);
  i2c_stop();
  
  i2c_start();
  i2c_write_addr(address + 1);
  *data = i2c_read(0);
  i2c_stop();
}