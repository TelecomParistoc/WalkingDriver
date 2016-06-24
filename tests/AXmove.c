#include <walkingdriver/driver.h>
#include <stdio.h>
#include <stdlib.h>

/* Test AX12 move callback */

int end = 0;

void onHigh();
void onLow() {
    printf("move finished\n");
    AX12move(129, 0, NULL);
    AX12move(146, 45, onHigh);
}
void onHigh() {
    AX12move(146, 0, onLow);
    AX12move(129, 45, onLow);
}

int main() {
    printf("init : %d\n", initAX12(115200));

    AX12setSpeed(146, 30);
    AX12move(146, 0, onLow);

    while(!end) {
        //printf("%f\n", AX12getPosition(146));
        waitFor(100);
    }
    return 0;
}
