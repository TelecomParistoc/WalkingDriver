#include <stdint.h>

#ifndef AXCOMM_H
#define AXCOMM_H

int initAXcomm(int baudrate);

int axWrite8(uint8_t id, uint8_t command, uint8_t arg, uint8_t* statusError);
int axWrite16(uint8_t id, uint8_t command, uint16_t arg, uint8_t* statusError);
int axRead8(uint8_t id, uint8_t command, uint8_t* arg, uint8_t* statusError);
int axRead16(uint8_t id, uint8_t command, uint16_t* arg, uint8_t* statusError);

#endif
