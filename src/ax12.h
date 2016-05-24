#ifndef AX12_H
#define AX12_H

#ifdef __cplusplus
extern "C" {
#endif

/* ====================== AX-12 ====================== */

/* Returns the position of the ax-12, */
int axGetPosition(int id);

// AX-12 FINISH CODES
#define MOVE_FINISHED 1
#define MOVE_BLOCKED 2
/* Returns the state of the corresponding flag and clears it if it was set
 * for axHasFinishedMove, returns 2 if the destination wasn't reached
 * (probably beacuse the ax-12 was blocked)
 *
 * Useless in endless turn mode and concerns the last ax-12 used */
int axHasFinishedMove();
int axIsForcing();

// AX-12 MODES
#define DEFAULT_MODE 0
#define WHEEL_MODE 1
/* High level function to control Ax-12 with id passed as parameter
 * Mustn't be called to modify settings of a moving ax-12
 * If you wan't to set speed or torque for a move, call this function
 * before the axMove function
 * If you don't want to change speed or torque, pass an invalid parameter
 * The last parameter enables you to chose the Ax-12 mode
 *
 * Torque : sets the maximum torque
 * Range  : 0 <= val <= 1023
 *
 * Speed  : sets the moving speed
 * Normal mode range :     0 <= val <= 1023
 * Wheel  mode range : -1023 <= val <= 1023
 *     a positive value is uses for a CounterClockWise rotation
 *     a negative value is used for a ClockWise rotation
 *
 * Mode : 0 for the Normal mode, 1 for Wheel mode (endless turn mode) */
void axSetTorqueSpeed(int id, int torque, int speed, int mode);


/* Moves Ax-12 with id passed as parameter to position (range 0 - 1023)
 * and calls callback() after the end of the move or after timeout milliseconds*/
void axMove(int id, int position, void (* callback) (void), int timeout);

void axStopPolling();

/* Calls the PIC routine that initializes all the Ax-12 */
void axReset();

#ifdef __cplusplus
}
#endif

#endif
