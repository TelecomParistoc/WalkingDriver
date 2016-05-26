#include "toolboxdriver.h"
#include "i2c-cache.h"
#include "i2c-functions.h"
#include <wiringPi.h>
#include <stdio.h>

// toolbox controller I2C address
#define TOOLBOX_ADDR 0x42
// writable 8 bit registers
#define TB_PWM1 0x41
#define TB_PWM2 0x42
#define TB_PWM3 0x43
#define TB_PWM4 0x44
#define TB_LEDS 0x45
#define TB_LEDSSO 0x46
#define TB_LEDSCO 0x47
#define TB_COLLMASK 0x48
#define TB_SENMASK 0x49
// readable 8 bit registers
#define TB_INTERRUPT_STATUS 0x40
#define TB_MOTOR_POWER_LEVEL 0x41
#define TB_LOGIC_POWER_LEVEL 0x42
#define TB_BUTTONS 0x43
#define TB_SENSORS 0x44
#define TB_COLLISIONS 0x45
// input pins
#define TB_INT 7
#define TB_BT4 2
#define TB_BT5 3
#define TB_BT6 0
// interrupt flags
#define SENSOR_CHANGE      0x04
#define COLLISION_CHANGE   0x08

// coefficient for motor battery level
#define MOTOR_LEVEL_COEFF 7965.511364
#define MOTOR_LEVEL_OFFSET 0.539773
// coefficient for motor battery level
#define LOGIC_LEVEL_COEFF 0.013736
#define LOGIC_LEVEL_OFFSET 0.082418

struct device_cache *tb_cache = NULL;

static uint8_t sensorsMask = 0x1F;
static uint8_t collisionsMask = 0x1F;

static void (*sensorsCallback)(void) = NULL;
static void (*collisionsCallback)(void) = NULL;

static void invalidateCache(int command) {
	tb_cache->r8_flags[command&0x0F] = CACHE_NOT_VALID;
}

void axInterruptManager(uint8_t flags);
void initAX12();

static void interruptManager() {
	if(digitalRead(TB_INT)) {
		uint8_t flags = I2Cread8(TOOLBOX_ADDR, TB_INTERRUPT_STATUS);
		// AX12 related interrupts
		axInterruptManager(flags);
		// other interrupts
		if(flags & SENSOR_CHANGE) {
			invalidateCache(TB_SENSORS);
			if(sensorsCallback != NULL)
				sensorsCallback();
		}
		if(flags & COLLISION_CHANGE) {
			invalidateCache(TB_COLLISIONS);
			if(collisionsCallback != NULL)
				collisionsCallback();
		}
	}
}

int initToolboxDriver() {
	tb_cache = initCache(TOOLBOX_ADDR, 6, 2, 10, 7);

	tb_cache->w8_cmds[TB_PWM1&0x0F] = TB_PWM1;
	tb_cache->w8_cmds[TB_PWM2&0x0F] = TB_PWM2;
	tb_cache->w8_cmds[TB_PWM3&0x0F] = TB_PWM3;
	tb_cache->w8_cmds[TB_PWM4&0x0F] = TB_PWM4;
	tb_cache->w8_cmds[TB_LEDS&0x0F] = TB_LEDS;
	tb_cache->w8_cmds[TB_LEDSSO&0x0F] = TB_LEDSSO;
	tb_cache->w8_cmds[TB_LEDSCO&0x0F] = TB_LEDSCO;
	tb_cache->w8_cmds[TB_COLLMASK&0x0F] = TB_COLLMASK;
	tb_cache->w8_cmds[TB_SENMASK&0x0F] = TB_SENMASK;

	tb_cache->r8_cmds[TB_INTERRUPT_STATUS&0x0F] = TB_INTERRUPT_STATUS;
	tb_cache->r8_cmds[TB_MOTOR_POWER_LEVEL&0x0F] = TB_MOTOR_POWER_LEVEL;
	tb_cache->r8_cmds[TB_LOGIC_POWER_LEVEL&0x0F] = TB_LOGIC_POWER_LEVEL;
	tb_cache->r8_cmds[TB_BUTTONS&0x0F] = TB_BUTTONS;
	tb_cache->r8_cmds[TB_SENSORS&0x0F] = TB_SENSORS;
	tb_cache->r8_cmds[TB_COLLISIONS&0x0F] = TB_COLLISIONS;

	tb_cache->updateCallback = interruptManager;

	// init AX12 I2C commands
	initAX12();

	// setup input pins
	wiringPiSetup();

	pinMode(TB_INT, INPUT);
	pinMode(TB_BT4, INPUT);
	pinMode(TB_BT5, INPUT);
	pinMode(TB_BT6, INPUT);
	pullUpDnControl(TB_INT, PUD_DOWN);
	pullUpDnControl(TB_BT4, PUD_DOWN);
	pullUpDnControl(TB_BT5, PUD_DOWN);
	pullUpDnControl(TB_BT6, PUD_DOWN);

	//check for the I2C connection
	if(i2c_init(TOOLBOX_ADDR) < 0) {
		printf("initToolboxDriver : ERROR cannot initialize Toolbox Controller\n");
		return -1;
	} else
		return 0;
}

double getMotorPowerLevel() {
	uint8_t val = c_read8(tb_cache, TB_MOTOR_POWER_LEVEL&0x0F);
	double voltage = 0;
	if(val) {
		double period = (val+300)*2;
		voltage = MOTOR_LEVEL_COEFF/period + MOTOR_LEVEL_OFFSET;
	}
	return voltage;
}

double getLogicPowerLevel() {
	uint8_t val = c_read8(tb_cache, TB_LOGIC_POWER_LEVEL&0x0F);
	return (val*2 + 448)*LOGIC_LEVEL_COEFF + LOGIC_LEVEL_OFFSET;
}

int getButton(int number) {
	if(number < 1 || number > 6) {
		printf("Button %d doesn't exist !\n", number);
		return -1;
	}
	if(number == 4)
		return digitalRead(TB_BT4);
	if(number == 5)
		return digitalRead(TB_BT5);
	if(number == 6)
		return digitalRead(TB_BT6);

	uint8_t val = c_read8(tb_cache, TB_BUTTONS&0x0F);
	if(val & (0x01 << (number-1)))
		return 1;
	else
		return 0;
}

int getSensor(int number) {
	if(number < 1 || number > 5) {
		printf("SENSOR %d doesn't exist !\n", number);
		return 0;
	}
	uint8_t val = c_read8(tb_cache, TB_SENSORS&0x0F);
	if(val & (0x01 << (number-1)))
		return 1;
	else
		return 0;
}

int getCollisionDetector(int number) {
	if(number < 1 || number > 5) {
		printf("Collision detector %d doesn't exist !\n", number);
		return -1;
	}
	uint8_t val = c_read8(tb_cache, TB_COLLISIONS&0x0F);
	if(val & (0x01 << (number-1)))
		return 1;
	else
		return 0;
}

void enableSensorCallback(int number) {
	if(number < 1 || number > 5) {
		printf("SENSOR %d doesn't exist !\n", number);
		return;
	}
	sensorsMask |= 1 << (number - 1);
	c_write8(tb_cache, TB_SENMASK&0x0F, sensorsMask);
}
void disableSensorCallback(int number) {
	if(number < 1 || number > 5) {
		printf("SENSOR %d doesn't exist !\n", number);
		return;
	}
	sensorsMask &= 0xFF ^ (1 << (number - 1));
	c_write8(tb_cache, TB_SENMASK&0x0F, sensorsMask);
}
void enableCollisionCallback(int number) {
	if(number < 1 || number > 5) {
		printf("Collision detector %d doesn't exist !\n", number);
		return;
	}
	collisionsMask |= 1 << (number - 1);
	c_write8(tb_cache, TB_COLLMASK&0x0F, collisionsMask);
}
void disableCollisionCallback(int number) {
	if(number < 1 || number > 5) {
		printf("Collision detector %d doesn't exist !\n", number);
		return;
	}
	collisionsMask &= 0xFF ^ (1 << (number - 1));
	c_write8(tb_cache, TB_COLLMASK&0x0F, collisionsMask);
}

void setPWM(int number, uint8_t value) {
	double dc = value*0.754;
	if(number < 1 || number > 4) {
		printf("the PWM out %d doesn't exist !\n", number);
		return;
	}
	c_write8(tb_cache, number, (uint8_t) dc);
}

void setLED(int number, int state) {
	if(number < 1 || number > 4) {
		printf("LED%d doesn't exist !\n", number);
		return;
	}
	if(state) {
		// if some data is already in cache
		if(tb_cache->w8_flags[TB_LEDSSO&0x0F]==CACHE_VALID)
			tb_cache->w8_cache[TB_LEDSSO&0x0F] |= 1 << (number - 1);
		else
			c_write8(tb_cache, TB_LEDSSO&0x0F, 1 << (number - 1));
	} else {
		// if some data is already in cache
		if(tb_cache->w8_flags[TB_LEDSCO&0x0F]==CACHE_VALID)
			tb_cache->w8_cache[TB_LEDSCO&0x0F] |= 1 << (number - 1);
		else
			c_write8(tb_cache, TB_LEDSCO&0x0F, 1 << (number - 1));
	}
}

void setCollisionsCallback(void (*callback)(void)) {
	collisionsCallback = callback;
}

void setSensorsCallback(void (*callback)(void)) {
	sensorsCallback = callback;
}
