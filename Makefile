TARGET = libwalkingdriver.so
SRCS = i2c-cache.c imudriver.c i2c-functions.c toolboxdriver.c timing.c ax12.c
HEADERS = $(addprefix src/, ${SRCS:.c=.h}) src/driver.h
OBJECTS = $(addprefix build/,${SRCS:.c=.o})
EXAMPLES =
TESTS = tests/timing tests/IMU tests/toolbox test/powertest
CC=gcc
CFLAGS = -O2 -std=gnu99 -Wall -Werror -fpic
LDFLAGS= -shared -lwiringPi -lm
PREFIX = /usr/local
VPATH = build/

vpath %.c src/ tests/ examples/
vpath %.h src/

.PHONY: clean test update small

all: build build/$(TARGET)

build:
	mkdir -p build
build/%.o: %.c build/%.d
	$(CC) -c -o $@ $< $(CFLAGS)
build/%.d : %.c
	$(CC) $(CFLAGS) -MM -MF $@ -MP $<

build/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

examples: LDFLAGS=-lwalkingdriver -lwiringPi
examples: $(EXAMPLES)

tests: LDFLAGS=-lwalkingdriver
tests: $(TESTS)

update:
	git pull
	make
	make install

clean:
	rm -f build/*.o build/*.so build/*.d
	rm -f $(TESTS)
	rm -f $(EXAMPLES)
jsinstall:
	mkdir -p $(DESTDIR)$(PREFIX)/lib/node_modules/walkingdriver
	cp JSbinding/* $(DESTDIR)$(PREFIX)/lib/node_modules/walkingdriver
	cd $(DESTDIR)$(PREFIX)/lib/node_modules/walkingdriver; npm install

install: build/$(TARGET) jsinstall
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	mkdir -p $(DESTDIR)$(PREFIX)/include/walkingdriver
	cp build/$(TARGET) $(DESTDIR)$(PREFIX)/lib/
	cp $(HEADERS) $(DESTDIR)$(PREFIX)/include/walkingdriver/
	chmod 0755 $(DESTDIR)$(PREFIX)/lib/$(TARGET)
	ldconfig
	ldconfig -p | grep walkingdriver

-include $(subst .c,.d,$(SRCS))
