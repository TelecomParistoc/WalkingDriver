#include "ax12.h"
#include "i2c-cache.h"
#include "i2c-functions.h"
#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>

// toolbox controller I2C address
#define TOOLBOX_ADDR 0x42
// writable 16 bit registers
#define AX_SETSPEED 0x81
#define AX_SETPOSITION 0x82
#define AX_SETTORQUE 0x83
#define AX_SETACTIVEWHEEL 0x84
#define AX_SETACTIVEDEFAULT 0x85
#define AX_RESET 0x86
// readable 16 bit registers
#define AX_GETPOSITION 0x81
// interrupt flags
#define AX12_FINISHED_MOVE 0x01
#define AX12_FORCING       0x02

extern struct device_cache *tb_cache;

static volatile int axFinishedMove = 0;
static volatile int axForcing = 0;

static void (*axCallback)(void) = NULL;

static int axCurrentId = 254;
static int axCurrentMode = 2;
static int axCurrentGoal = 2000;
static int axTimeout = 100;
static int axTimeMoved = 101;
static int hasTimeouted = 0;

void axInterruptManager(uint8_t flags) {
    if(flags & AX12_FINISHED_MOVE) {
		int currentPos = I2Cread16(TOOLBOX_ADDR, AX_GETPOSITION);
		axTimeMoved = axTimeout + 1;
		if((axCurrentGoal - currentPos < 5) && (axCurrentGoal - currentPos > -5))
			axFinishedMove = 1;
		else
			axFinishedMove = 2;
		if(axCallback != NULL && !hasTimeouted)
			axCallback();
	} else {
		if(axTimeMoved == axTimeout){
			hasTimeouted = 1;
			axFinishedMove = 2;
			if(axCallback != NULL)
				axCallback();
		}
		axTimeMoved ++;
	}
	if(flags & AX12_FORCING) {
		axForcing = 1;
	}
}

// init AX12 I2C commands
void initAX12() {
    // !!! dont forget to modify initCache arguments in initToolboxDriver when commands are modified !!!
    tb_cache->w16_cmds[AX_SETSPEED&0x0F] = AX_SETSPEED;
    tb_cache->w16_cmds[AX_SETPOSITION&0x0F] = AX_SETPOSITION;
    tb_cache->w16_cmds[AX_SETTORQUE&0x0F] = AX_SETTORQUE;
    tb_cache->w16_cmds[AX_SETACTIVEWHEEL&0x0F] = AX_SETACTIVEWHEEL;
    tb_cache->w16_cmds[AX_SETACTIVEDEFAULT&0x0F] = AX_SETACTIVEDEFAULT;
    tb_cache->w16_cmds[AX_RESET&0x0F] = AX_RESET;

    tb_cache->r16_cmds[AX_GETPOSITION&0x0F] = AX_GETPOSITION;
}

int axGetPosition(int id) {
	if((axCurrentId != id) || axCurrentMode )
		setAxActiveDefault(id);
	int val = c_read16(tb_cache, AX_GETPOSITION&0x0F);
	return val;
}

void setAxActiveWheel(uint8_t id) {
	I2Cwrite16(TOOLBOX_ADDR, AX_SETACTIVEWHEEL, id);
	axCurrentId = id;
	axCurrentMode = WHEEL_MODE;
	axCurrentGoal = 2000;
}


void setAxActiveDefault(uint8_t id) {
	I2Cwrite16(TOOLBOX_ADDR, AX_SETACTIVEDEFAULT, id);
	axCurrentId = id;
	axCurrentMode = DEFAULT_MODE;
	axCurrentGoal = 2000;
}

void setAxSpeed(int speed) {
	I2Cwrite16(TOOLBOX_ADDR, AX_SETSPEED, speed);
}

void setAxPosition(int position) {
	I2Cwrite16(TOOLBOX_ADDR, AX_SETPOSITION, position);
	axCurrentGoal = position;
}

void setAxTorque(int torque) {
	I2Cwrite16(TOOLBOX_ADDR, AX_SETTORQUE, torque);
}

void axReset() {
	I2Cwrite16(TOOLBOX_ADDR, AX_RESET, 0);
}

int axHasFinishedMove() {
	if (axFinishedMove) {
		int tmp = axFinishedMove;
		axFinishedMove = 0;
		return tmp;
	}
	return 0;
}

int axIsForcing() {
	if (axForcing) {
		printf("Forcing\n");
		axForcing = 0;
		return 1;
	}
	return 0;
}

void axSetTorqueSpeed(int id, int torque, int speed, int mode) {
	if ((id != axCurrentId) || (mode != axCurrentMode)){
		if(mode)
			setAxActiveWheel(id);
		else
			setAxActiveDefault(id);
	}

	if ((torque >= 0) && (torque <= 1023))
		setAxTorque(torque);

	if (mode){
		if((speed >= -1023) && (speed <= 1023)){
			if(speed >= 0)
				setAxSpeed(speed);
			else
				setAxSpeed(1024 - speed);
		}
	}
	else{
		if((speed >= 0) && (speed <= 1023))
			setAxSpeed(speed);
	}

	axFinishedMove = 0;
}

void axMove(int id, int position, void (* callback) (void), int timeout) {
	axTimeMoved = 0;
	axTimeout = timeout / 10;
	if ((id != axCurrentId) || axCurrentMode)
		setAxActiveDefault(id);
	axFinishedMove = 0;
	if ((position >= 0 && position <= 1023)){
		axCallback = callback;
		hasTimeouted = 0;
		setAxPosition(position);
	}
	else{
		printf("Position %d is out of range\n", position);
	}
}

void axStopPolling() {
	axSetTorqueSpeed(0, -1, 2000, 1);
}
