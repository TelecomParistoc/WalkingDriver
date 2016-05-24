#ifndef I2CDRIVERCACHE_H
#define I2CDRIVERCACHE_H

#include <stdint.h>

#define CACHE_NOT_VALID 2
#define CACHE_RECENT_READ 1
#define CACHE_NO_READ 0
#define CACHE_VALID 0

struct device_cache {
    int8_t addr;
    void (*updateCallback)(void);
    int r8_cache_length;
    int8_t *r8_cmds;
    int8_t *r8_cache;
    int8_t *r8_flags;
    int r16_cache_length;
    int8_t *r16_cmds;
    int16_t *r16_cache;
    int8_t *r16_flags;
    int w8_cache_length;
    int8_t *w8_cmds;
    int8_t *w8_cache;
    int8_t *w8_flags;
    int w16_cache_length;
    int8_t *w16_cmds;
    int16_t *w16_cache;
    int8_t *w16_flags;
};

struct device_cache* initCache(int8_t dev_addr, int r8_len, int r16_len, int w8_len, int w16_len);

int8_t c_read8(struct device_cache *cache, int cmd_index);
int16_t c_read16(struct device_cache *cache, int cmd_index);
int c_write8(struct device_cache *cache, int cmd_index, int8_t value);
int c_write16(struct device_cache *cache, int cmd_index, int16_t value);

/* set updates frequency (in Hz) : has to be between 1 and 1000 hz
 * otherwise frequency will be set to the closer valid value */
void setUpdateFrequency(int frequency);
/* get current update frequency */
int getUpdateFrequency();

void startLoop();

#endif
