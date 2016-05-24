#include "i2c-cache.h"
#include "i2c-functions.h"
#include "timing.h"
#include <sys/time.h>
#include <stdio.h>

#define SCHEDULED_MAX 40

static void (*scheduledCallbacks[SCHEDULED_MAX])(void) = {NULL};
static long long int scheduledTimes[SCHEDULED_MAX] = {-1};

static long long int getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long int currentTime = tv.tv_usec/1000 + tv.tv_sec;
    return currentTime;
}

//called each period using I2C cache timer
void timingManager() {
    long long int currentTime = getCurrentTime();
    // call scheduled callbacks
    for(int i=0; i<SCHEDULED_MAX; i++) {
        if(scheduledTimes[i] > 0 && (scheduledTimes[i] - currentTime) <= 0) {
            scheduledTimes[i] = -1;
            if(scheduledCallbacks[i] != NULL)
                scheduledCallbacks[i]();
        }
    }
}

void waitFor(int milliseconds) {
	delayMilli(milliseconds);
}

int scheduleIn(int milliseconds, void (*callback)(void)) {
	int i=0;
	while(i<SCHEDULED_MAX && scheduledTimes[i]>0)
		i++;
	if(i==SCHEDULED_MAX) {
        printf("ERROR : scheduled functions buffer full, cannot schedule\n");
        return -1;
    }

	scheduledCallbacks[i] = callback;
	scheduledTimes[i] = getCurrentTime() + milliseconds;

    //make sure loop is started
    startLoop();
	return 0;
}
