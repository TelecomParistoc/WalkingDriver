/* IMU (Inertial Motion Unit) driver for bno055
 * keeps track of the robot's relative orientation
 * functionnalities :
 *  > get/set heading, roll and pitch
 *
 *  Note : in order to free the programmer from I2C ressource sharing
 * considerations, read may be cached and write may be delayed when write/read
 * are made too often and might saturate the I2C bus.
 */

#ifndef I2CIMUDRIVER_H
#define I2CIMUDRIVER_H


/* initialize the bno055 and set it in IMU mode (fusion between accelerometer and gyro)
 * this has to be called before any other operation, UNLESS the initMotorDriver()
 * was called : in that case calling initIMU() is redundant.
 * returns 0 if success, -1 if failure */
int initIMU();

/* get/set euler angles. the angles are relative to the power-on position of the
 * module. All angles are in degrees from 0 to 360 (excluded)
 * WARNING : setting angles DOES NOT MOVE the robot, it offsets the angle*/

double getHeading();
void setHeading(double heading);
double getPitch();
void setPitch(double pitch);
double getRoll();
void setRoll(double roll);

/* change heading rotation direction (usefull for team change). 0 is clockwise
 * (default) and 1 is counterclockwise */
void setHeadingRotationDirection(int direction);
int getHeadingRotationDirection();

#endif
