#include "i2c-cache.h"
#include "i2c-functions.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>

static struct device_cache *caches[10];
static int cacheCount = 0;
static int updating = 0;
static int period = 10000;

void timingManager();

static int8_t* allocTable8(int size, int8_t fill) {
	int8_t *table = malloc(sizeof(int8_t)*size);
	int i = 0;
	for(;i<size;i++)
		table[i] = fill;
	return table;
}

static int16_t* allocTable16(int size, int16_t fill) {
	int16_t *table = malloc(sizeof(int16_t)*size);
	int i = 0;
	for(;i<size;i++)
		table[i] = fill;
	return table;
}

struct device_cache* initCache(int8_t dev_addr, int r8_len, int r16_len, int w8_len, int w16_len) {
	struct device_cache* cache = malloc(sizeof(struct device_cache));
	cache->addr = dev_addr;
	cache->updateCallback = NULL;
	if(r8_len == 0)
		r8_len = 1;
	cache->r8_cache_length = r8_len;
	cache->r8_cmds = allocTable8(r8_len, 0x00);
	cache->r8_cache = allocTable8(r8_len, 0x00);
	cache->r8_flags = allocTable8(r8_len, CACHE_NOT_VALID);
	if(r16_len == 0)
		r16_len = 1;
	cache->r16_cache_length = r16_len;
	cache->r16_cmds = allocTable8(r16_len, 0x00);
	cache->r16_cache = allocTable16(r16_len, 0x00);
	cache->r16_flags = allocTable8(r16_len, CACHE_NOT_VALID);
	if(w8_len == 0)
		w8_len = 1;
	cache->w8_cache_length = w8_len;
	cache->w8_cmds = allocTable8(w8_len, 0x00);
	cache->w8_cache = allocTable8(w8_len, 0x00);
	cache->w8_flags = allocTable8(w8_len, CACHE_NOT_VALID);
	if(w16_len == 0)
		w16_len = 1;
	cache->w16_cache_length = w16_len;
	cache->w16_cmds = allocTable8(w16_len, 0x00);
	cache->w16_cache = allocTable16(w16_len, 0x00);
	cache->w16_flags = allocTable8(w16_len, CACHE_NOT_VALID);

	if(cacheCount > 9)
		cacheCount = 9;
	caches[cacheCount] = cache;
	cacheCount++;

	// raise priority of the process
	if(nice(0) < 0)
		printf("cannot raise process priority\n");

	return cache;
}

static void updateCache(struct device_cache *cache) {
	int i;
	for(i=0; i<cache->r8_cache_length; i++) {
		if(cache->r8_flags[i] == CACHE_RECENT_READ) { // if a read was made after last update
			cache->r8_cache[i] = I2Cread8(cache->addr, cache->r8_cmds[i]); // we update the current value
			updating = 1;
			cache->r8_flags[i] = CACHE_NO_READ;
		} else if(cache->r8_flags[i] == CACHE_NO_READ) { // otherwise
			cache->r8_flags[i] = CACHE_NOT_VALID; // stop updating
		}
	}
	for(i=0; i<cache->r16_cache_length; i++) {
		if(cache->r16_flags[i] == CACHE_RECENT_READ) { // if a read was made after last update
			cache->r16_cache[i] = I2Cread16(cache->addr, cache->r16_cmds[i]); // we update the current value
			updating = 1;
			cache->r16_flags[i] = CACHE_NO_READ;
		} else if(cache->r16_flags[i] == CACHE_NO_READ) { // otherwise
			cache->r16_flags[i] = CACHE_NOT_VALID; // stop updating
		}
	}

	if(cache->updateCallback != NULL)
		cache->updateCallback();

	for(i=0; i<cache->w8_cache_length; i++) {
		if(cache->w8_flags[i] != CACHE_NOT_VALID) { // if a write was made after last update
			I2Cwrite8(cache->addr, cache->w8_cmds[i], cache->w8_cache[i]); // we update the current value
			updating = 1;
			cache->w8_flags[i] = CACHE_NOT_VALID;
		}
	}
	for(i=0; i<cache->w16_cache_length; i++) {
		if(cache->w16_flags[i] != CACHE_NOT_VALID) { // if a write was made after last update
			I2Cwrite16(cache->addr, cache->w16_cmds[i], cache->w16_cache[i]); // we update the current value
			updating = 1;
			cache->w16_flags[i] = CACHE_NOT_VALID;
		}
	}
}
static void startTimer(int arg) {
	struct itimerval timerDelay;
	timerDelay.it_interval.tv_sec = 0;
	timerDelay.it_interval.tv_usec = 0;
	timerDelay.it_value.tv_sec = 0;
	timerDelay.it_value.tv_usec = period;

	if(arg != 23)
		printf("WARNING : cache loop is taking too long ! Optimize the loop or choose a longer period\n");

	signal(SIGALRM, startTimer);
	setitimer(ITIMER_REAL, &timerDelay, NULL);
}
static void onUpdate(int arg) {
	startTimer(23);

	for(int i=0; i<cacheCount; i++)
		updateCache(caches[i]);
	timingManager();

	signal(SIGALRM, onUpdate);
}
static void startUpdates() {
	struct itimerval timerDelay;
	timerDelay.it_interval.tv_sec = 0;
	timerDelay.it_interval.tv_usec = 0;
	timerDelay.it_value.tv_sec = 0;
	timerDelay.it_value.tv_usec = period/2;
	signal(SIGALRM, onUpdate);
	setitimer(ITIMER_REAL, &timerDelay, NULL);
	updating = 1;
}

void setUpdateFrequency(int frequency) {
	period = 1000000/frequency;
	if(period >= 1000000)
		period = 999999;
	else if(period < 1000)
		period = 1000;
}
int getUpdateFrequency() {
	return 1000000/period;
}

int8_t c_read8(struct device_cache *cache, int cmd_index) {
	if(cache == NULL) {
		printf("c_read8 : ERROR cache not initialized !\n");
		return -2;
	}
	if(cmd_index >= cache->r8_cache_length) {
		printf("c_read8 : ERROR out of bound command index !\n");
		return -1;
	}
	if(cache->r8_flags[cmd_index] == CACHE_NOT_VALID)
		cache->r8_cache[cmd_index] = I2Cread8(cache->addr, cache->r8_cmds[cmd_index]);
	if(!updating)
		startUpdates();
	cache->r8_flags[cmd_index] = CACHE_RECENT_READ;
	return cache->r8_cache[cmd_index];
}
int16_t c_read16(struct device_cache *cache, int cmd_index) {
	if(cache == NULL) {
		printf("c_read16 : ERROR cache not initialized !\n");
		return -2;
	}
	if(cmd_index >= cache->r16_cache_length) {
		printf("c_read16 : ERROR out of bound command index !\n");
		return -1;
	}
	if(cache->r16_flags[cmd_index] == CACHE_NOT_VALID)
		cache->r16_cache[cmd_index] = I2Cread16(cache->addr, cache->r16_cmds[cmd_index]);
	if(!updating)
		startUpdates();
	cache->r16_flags[cmd_index] = CACHE_RECENT_READ;
	return cache->r16_cache[cmd_index];
}
int c_write8(struct device_cache *cache, int cmd_index, int8_t value) {
	if(cache == NULL) {
		printf("c_write8 : ERROR cache not initialized !\n");
		return -2;
	}
	if(cmd_index >= cache->w8_cache_length) {
		printf("c_write8 : ERROR out of bound command index !\n");
		return -1;
	}
	cache->w8_cache[cmd_index] = value;
	cache->w8_flags[cmd_index] = CACHE_VALID;
	if(!updating)
		startUpdates();
	return 0;
}
int c_write16(struct device_cache *cache, int cmd_index, int16_t value) {
	if(cache == NULL) {
		printf("c_write16 : ERROR cache not initialized !\n");
		return -2;
	}
	if(cmd_index >= cache->w16_cache_length) {
		printf("c_write16 : ERROR out of bound command index !\n");
		return -1;
	}
	cache->w16_cache[cmd_index] = value;
	cache->w16_flags[cmd_index] = CACHE_VALID;
	if(!updating)
		startUpdates();
	return 0;
}

void startLoop() {
	if(!updating)
		startUpdates();
}
