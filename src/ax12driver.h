/* this file provides control over the AX12 servos.
 * WARNING : it is required to call initAX12 before any operations */

#ifndef AX12_H
#define AX12_H

#ifdef __cplusplus
extern "C" {
#endif

/* initialize the AX12 driver. Calling this is MANDATORY for proper use */
int initAX12(int baudrate);

/* get AX12 position output position in degree from -150 to 150, increasing
 * clockwise (when looking at the front side of the AX12). */
double AX12getPosition(uint8_t id);
/* get current output speed in %, from -100 to 100, positive speed is clockwise.
 * Current speed may be inaccurate and should not be relied on */
double AX12getSpeed(uint8_t id);
/* get current output load in %, from -100 to 100, positive load means rotating
 * clockwise. Current load may be inaccurate and should not be relied on */
double AX12getLoad(uint8_t id);
/* get current AX12 status (error flags). See datasheet for more info */
int AX12getStatus(uint8_t id);
/* get AX12 power voltage (in volts) */
double AX12getVoltage(uint8_t id);
/* get AX12 temperature (in degrees celsius) */
int AX12getTemperature(uint8_t id);
/* returns 1 if AX12 is moving by its own power, 0 otherwise */
int AX12isMoving(uint8_t id);

// AX-12 MODES
#define DEFAULT_MODE 0
#define WHEEL_MODE 1
/* set AX12 control mode : default mode allows position control, wheel mode allows
 * endless turn */
void AX12setMode(uint8_t id, int mode);
/* set AX12 goal speed from -100 to 100, positive speed is clockwise (when
 * looking at the front side of the AX12) */
void AX12setSpeed(uint8_t id, double speed);
/* set AX12 max torque from 0 to 100. Setting torque to zero will disable torque
 * (preventing AX12 from moving), setting to any other value will enable torque */
void AX12setTorque(uint8_t id, double torque);
/* set rear LED state, 1 for ON, 0 for OFF */
void AX12setLED(uint8_t id, int state);
/* move to a given position in degree from -150 to 150, increasing clockwise
 * (when looking at the front side of the AX12). It will change mode to default
 * if necessary. A callback can be called when the AX12 stops moving (when it
 * reached the goal position or if an error occured) */
void AX12move(uint8_t id, double position, void (*callback)(void));
/* cancel an end move callback for a given AX12 */
void AX12cancelCallback(uint8_t id);
/* switch to wheel mode and set speed (from -100 to 100, positive is clockwise) */
void AX12turn(uint8_t id, double speed);

/* reset all AX12 to a default config (including enable and set torque to maximum,
 * speed at 50%, default mode) */
void AX12resetAll();

#ifdef __cplusplus
}
#endif

#endif
