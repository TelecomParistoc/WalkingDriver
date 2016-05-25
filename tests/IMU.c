#include <walkingdriver/driver.h>
#include <stdio.h>
#include <stdlib.h>

/* Test IMU driver */

int main() {
    printf("Initializing IMU ... ");
    if(initIMU()) {
        printf("failed\n");
        return -1;
    }
    printf("OK\n");

    for (int i = 0; i < 100; i++) {
        printf("Heading = %fdeg, Pitch = %fdeg, Roll = %fdeg\r", getHeading(), getPitch(), getRoll());
        waitFor(100);
    }
    printf("Resetting angles ...\n");
    setHeading(0);
    setPitch(0);
    setRoll(0);
	while(1) {
        printf("Heading = %fdeg, Pitch = %fdeg, Roll = %fdeg\r", getHeading(), getPitch(), getRoll());
        waitFor(100);
    }
	return 0;
}
