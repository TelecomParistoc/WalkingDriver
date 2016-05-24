#include <stdint.h>
#include <stdio.h>

#include "imudriver.h"
#include "bno055.h"
#include "i2c-cache.h"
#include "i2c-functions.h"

#define IMU_HEADING 0
#define IMU_ROLL 1
#define IMU_PITCH 2

static struct device_cache *cache = NULL;

static double headingOffset = 0.0;
static double pitchOffset = 0.0;
static double rollOffset = 0.0;

static int headingRotationDirection = 0;

static void setMode(bno055_opmode_t mode) {
  I2Cwrite8(BNO055_ADDRESS, BNO055_OPR_MODE_ADDR, mode);
  delayMilli(30);
}

int initIMU() {
    /* Make sure we have the right device */
    uint8_t id = I2Cread8(BNO055_ADDRESS, BNO055_CHIP_ID_ADDR);
    if(id != BNO055_ID) {
        printf("initIMU : ERROR wrong device ID, check IMU is connected to the I2C bus\n");
        return -1;  // no? ok bail
    }

    /* Switch to config mode (just in case since this is the default) */
    setMode(OPERATION_MODE_CONFIG);

    /* Reset */
    I2Cwrite8(BNO055_ADDRESS, BNO055_SYS_TRIGGER_ADDR, 0x20);
    while (((uint8_t) I2Cread8(BNO055_ADDRESS, BNO055_CHIP_ID_ADDR)) != BNO055_ID) {
        delayMilli(10);
    }
    delayMilli(50);

    /* Set to normal power mode */
    I2Cwrite8(BNO055_ADDRESS, BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL);
    delayMilli(10);

    I2Cwrite8(BNO055_ADDRESS, BNO055_PAGE_ID_ADDR, 0);

    I2Cwrite8(BNO055_ADDRESS, BNO055_SYS_TRIGGER_ADDR, 0x0);
    delayMilli(10);
    /* Set the  operating mode (see section 3.3) */
    setMode(OPERATION_MODE_IMUPLUS);
    delayMilli(100);

    /* set up the cache system */
    cache = initCache(BNO055_ADDRESS, 0, 3, 0, 0);
    cache->r16_cmds[IMU_HEADING] = BNO055_EULER_H_LSB_ADDR;
    cache->r16_cmds[IMU_ROLL] = BNO055_EULER_R_LSB_ADDR;
    cache->r16_cmds[IMU_PITCH] = BNO055_EULER_P_LSB_ADDR;

    return 0;
}

double getHeading() {
    int val = c_read16(cache, IMU_HEADING) & 0x1FFF;
    double result = val/16.0;

    if(headingRotationDirection)
        result = 360 -  result;

    result = result - headingOffset;
    if(result >= 360)
        result -= 360;
    if(result < 0)
        result += 360;
    return result;
}
void setHeading(double heading) {
    if(heading >= 0 && heading < 360) {
        headingOffset = 0;
        headingOffset = getHeading() - heading;
    }
}
void setHeadingRotationDirection(int direction) {
    headingRotationDirection = direction ? 1 : 0;
}
int getHeadingRotationDirection() {
    return headingRotationDirection;
}

double getPitch() {
    int val = c_read16(cache, IMU_PITCH);
    double result = val/16;
    result = result - pitchOffset;
    if(result >= 360)
        result -= 360;
    if(result < 0)
        result += 360;
    return result;
}
void setPitch(double pitch) {
    if(pitch >= 0 && pitch < 360) {
        pitchOffset = 0;
        pitchOffset = getPitch() - pitch;
    }
}
double getRoll() {
    int val = c_read16(cache, IMU_ROLL);
    double result = val/16;
    result = result - rollOffset;
    if(result >= 360)
        result -= 360;
    if(result < 0)
        result += 360;
    return result;
}
void setRoll(double roll) {
    if(roll >= 0 && roll < 360) {
        rollOffset = 0;
        rollOffset = getRoll() - roll;
    }
}
