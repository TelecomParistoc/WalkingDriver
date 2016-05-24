/* i2c functions
 * provides functions to acces the I2C bus
 */

#ifndef I2CDRIVERFUNCTIONS_H
#define I2CDRIVERFUNCTIONS_H
#include <stdint.h>

/* ############## I2C functions ##############
 * If the i2c device is not open, they will open it before the read/write
 * but it is also possible to do it manually with i2c_init()
 */

int i2c_init(uint8_t addr);

int8_t I2Cread8(uint8_t addr, uint8_t reg);
int16_t I2Cread16(uint8_t addr, uint8_t reg);
int I2Cwrite8(uint8_t addr, uint8_t reg, uint8_t value);
int I2Cwrite16(uint8_t addr, uint8_t reg, uint16_t value);

/* wait a given number of milliseconds*/
void delayMilli(int milliseconds);

#endif
