#include <walkingdriver/driver.h>
#include <stdio.h>
#include <stdlib.h>

/* Test AX12 move callback */

int end = 0;

void onEnd() {
    printf("move finished\n");
    end = 1;
}

int main() {
    printf("init : %d\n", initAX12(115200));

    AX12setSpeed(146, 200);
    AX12move(146, 512, onEnd);

    while(!end) {
        printf("%d\n", AX12getPosition(146));
        waitFor(100);
    }
    return 0;
}
