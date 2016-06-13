#include <walkingdriver/driver.h>
#include <stdio.h>
#include <stdlib.h>

/* a simple program to read an AX12 position */

int main() {
    int AX12_ID;
    initToolboxDriver();

    printf("Enter AX-12 ID : ");
    scanf("%d", &AX12_ID);
    if(AX12_ID > 254 || AX12_ID < 0) {
        printf("ID must be between 0 and 254\n");
        return -1;
    }

    // no speed, no torque, wheel mode
    axSetTorqueSpeed(AX12_ID, 0, 0, WHEEL_MODE);
    // first read to load position
    axGetPosition(AX12_ID);

    while(1) {
        waitFor(200);
        printf("AX-12 %d at position %d\n", AX12_ID, axGetPosition(AX12_ID));
    }
    return 0;
}
