#include <walkingdriver/driver.h>
#include <stdio.h>
#include <stdlib.h>

/* Test AX12 move callback */
void onHigh();
void onLow() {
    printf("move finished\n");
    AX12move(164, 20, onHigh);
}
void onHigh() {
    AX12move(164, -20, onLow);
}

int main() {
    printf("init : %d\n", initAX12(115200));

    AX12setSpeed(164, 30);
    AX12move(164, 0, onLow);

    while(1) {
        waitFor(100);
    }
    return 0;
}
