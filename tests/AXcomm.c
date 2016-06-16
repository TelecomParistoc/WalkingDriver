#include <walkingdriver/driver.h>
#include <walkingdriver/ax-comm.h>
#include <stdio.h>
#include <stdlib.h>

/* Test communication with AX12s */

int main() {
    uint16_t position;
    uint8_t error;
    int code;

    printf("init : %d\n", initAXcomm(115200));

    code = axWrite8(146, 0x19, 1, &error);
    printf("write8 : code = %d, error = %x\n", code, error);

    return 0;
}
