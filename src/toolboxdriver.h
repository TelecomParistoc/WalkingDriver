#ifndef TOOLBOXDRIVER_H
#define TOOLBOXDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* initialize the toolbox driver, needs to be called before any other operations
 * returns -1 if it was unable to init the toolbox driver, 0 otherwise*/
int initToolboxDriver();

/* returns the current voltage of the battery powering motors and AX-12s (in volts) */
double getMotorPowerLevel();
/* returns the current voltage of the battery powering the sensors,
 * motioncontrollers and the raspberry pi (in volts) */
double getLogicPowerLevel();

/* get the state of a button (1 is logic high and 0 logic low). Buttons 1, 2 & 3
 * are connected to the toolbox board and buttons 4 & 5 are connected on raspberry
 * Pi's pins 13 and 15 */
int getButton(int number);
/* get the state of a sensor (1 is logic high and 0 logic low).
 * number : a sensor from 1 to 5 */
int getSensor(int number);
/* get the state of a collision detector. Returns 0 or 1. Depending on the model,
 * logical states can have different meanings
 * number : a collision detector from 1 to 5 */
int getCollisionDetector(int number);

/* set a function called when a sensor changes. It will be called whenever the
 * state of a sensor changes (from 0 to 1 or from 1 to 0), if the sensor callback
 * is enabled. (by default all the sensors triggers the callback) */
void setSensorsCallback(void (*callback)(void));
/* allow a sensor to trigger the callback when its state changes
 * number : a sensor from 1 to 5 */
void enableSensorCallback(int number);
/* prevent a sensor from triggering the callback when its state changes
 * number : a sensor from 1 to 5 */
void disableSensorCallback(int number);

/* set a function called when a collision detector changes. It will be called whenever the
 * state of a sensor changes (from 0 to 1 or from 1 to 0), if the sensor callback
 * is enabled. (by default all the sensors triggers the callback) */
void setCollisionsCallback(void (*callback)(void));
/* allow a collision detector to trigger the callback when its state changes
 * number : a collision detector from 1 to 5 */
void enableCollisionCallback(int number);
/* prevent a collision detector from triggering the callback when its state changes
 * number : a collision detector from 1 to 5 */
void disableCollisionCallback(int number);

/* set the duty cycle of a PWM output.
 *    number : a PWM output, from 1 to 4
 *    value : the duty cycle, from 0 (always low) to 255 (always high) */
void setPWM(int number, uint8_t value);
/* set the state of a LED
 *      number : a LED from 1 to 4
 *      state : 1 to turn on, 0 to turn off */
void setLED(int number, int state);

#ifdef __cplusplus
}
#endif

#endif
