#include <walkingdriver/driver.h>
#include <stdio.h>
#include <stdlib.h>

/* Test timing functions */

float cnt = 0;
static void f() {
    printf("%d sec\n", cnt);
    cnt += 0.5;
}
static void g() {
    printf("I should never be called\n");
}

int main() {
    // check cancelScheduled works
    cancelScheduled(scheduleIn(500, f));
    // should print an ERROR then the time since start each 500ms up to 20sec
    for (int i = 1; i <= 41; i++) {
        scheduleIn(i*500, f);
    }
	while(1) {
        waitFor(500);
        printf("----------\n");
    }
	return 0;
}
