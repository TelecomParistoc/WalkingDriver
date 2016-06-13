#include <walkingdriver/driver.h>
#include <stdio.h>
#include <stdlib.h>

/* Calibrate battery power sensors */

int main() {
    char tmp;
    double motor10, logic10, motor12, logic12, motorCoef, logicCoef, motorOffset, logicOffset;
    initToolboxDriver();

    printf("ATTENTION : assurrez-vous que les coefficients sont à 1 et les offset à 0.\n");
    printf("réglez les tensions batteries et moteur à 10V\n");
    printf("presser entrée pour continuer ...");
    scanf("%c", &tmp);
    motor10 = getMotorPowerLevel();
    logic10 = getLogicPowerLevel();
    printf("réglez les tensions batteries et moteur à 12.5V\n");
    printf("presser entrée pour continuer ...");
    scanf("%c", &tmp);
    motor12 = getMotorPowerLevel();
    logic12 = getLogicPowerLevel();

    motorCoef = (motor12-motor10)/(12.5-10);
    logicCoef = (logic12-logic10)/(12.5-10);
    motorOffset = 10*motorCoef-motor10;
    motorOffset = 10*motorCoef-motor10;

    printf("MOTOR_LEVEL_COEFF = %f\n", motorCoef);
    printf("LOGIC_LEVEL_COEFF = %f\n", logicCoef);
    printf("MOTOR_LEVEL_OFFSET = %f\n", motorOffset);
    printf("LOGIC_LEVEL_OFFSET = %f\n", logicOffset);
    return 0;
}
