#include <walkingdriver/driver.h>
#include <stdio.h>
#include <stdlib.h>

static void collisionsCallback() {
	printf("coll1:%d, coll2:%d, coll3:%d, coll4:%d, coll5: %d\n",
        getCollisionDetector(1), getCollisionDetector(2), getCollisionDetector(3),
        getCollisionDetector(4), getCollisionDetector(5));
}
static void sensorsCallback() {
	printf("sensor1:%d, sensor2:%d, sensor3: %d, sensor4:%d, sensor5: %d\n",
        getSensor(1), getSensor(2), getSensor(3), getSensor(4), getSensor(5));
}

/* Test toolbox module driver */
int main() {
    printf("Initializing toolbox module ... ");
    if(initToolboxDriver()) {
        printf("failed\n");
        return -1;
    }
    printf("OK\n");

    // turn on some LEDs
	setLED(1, 1);
	setLED(2, 0);
	setLED(3, 1);

    // test PWM
    setPWM(1, 0);
    setPWM(2, 64);
    setPWM(3, 192);
    setPWM(4, 255);

    // enable callback only for sensors in use and call sensorsCallback when something happens
	enableSensorCallback(1);
	enableSensorCallback(2);
	enableSensorCallback(3);
	enableSensorCallback(4);
	enableSensorCallback(5);
	setSensorsCallback(sensorsCallback);

	// enable callback only for collision detectors in use and call collisionsCallback when something happens
	enableCollisionCallback(1);
	enableCollisionCallback(2);
	enableCollisionCallback(3);
	enableCollisionCallback(4);
	enableCollisionCallback(5);
	setCollisionsCallback(collisionsCallback);

	while(1) {
        printf("Logic battery: %f V, motor battery: %f V\n", getLogicPowerLevel(), getMotorPowerLevel());
        printf("button1:%d, button2:%d, button3: %d, button4:%d, button5: %d\n",
            getButton(1), getButton(2), getButton(3), getButton(4), getButton(5));
        waitFor(100);
    }
	return 0;
}
