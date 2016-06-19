# Walking robot drivers #

A library providing high level access to the walking robot capabilities.
This library controls robot's modules over I2C and serial port, and provides an advanced bus
ressource sharing mecanism.

It provides (user space) drivers for :

* the IMU (BNO055)
* the toolbox module, providing access to robot's sensors, buttons and LEDs
* AX12 servos through the GPIO serial port

## Installation ##

This library is designed for Raspberry Pi with Raspbian.

* First, you need [wiringPi](http://wiringpi.com/download-and-install/).

* Once wiringPi is installed, get back the sources :
`git clone git@github.com:TelecomParistoc/WalkingDriver.git`

* cd to the root of the repository and enter `make`

* finally enter `sudo make install`

## Usage ##

Don't forget to compile (actually, link) your C/C++ app with `-lwalkingdriver` option.
You can include all the headers you need with :
```c
#include "walkingdriver/driver.h"
```
It is also possible to include headers individually.

### IMU driver ###

Heading access is provided by motor module driver, but access to pitch and roll
is available through imudriver.h. Include the header with :

```c
#include "walkingdriver/imudriver.h"
```

For more info on imu driver API, see [imudriver.h](https://github.com/TelecomParistoc/WalkingDriver/blob/master/src/imudriver.h)

### Toolbox module driver ###

The toolbox module provides :
* 5 sensors inputs, supporting callback on change
* 5 collision detector inputs, supporting callback on change
* 3 button inputs
* 4 LEDs ouputs
* 4 PWM outputs
* voltage measurements of both batteries

To use these functions, include the headers :

```c
#include "walkingdriver/toolboxdriver.h"
```

For more info on toolbox module API, see [toolboxdriver.h](https://github.com/TelecomParistoc/WalkingDriver/blob/master/src/toolboxdriver.h)

### AX12 driver ###

The AX12 driver provides functions to control the AX12 through Raspberry Pi serial port (on the GPIO header).
Most of the AX12 features can be controlled via easy-to-use functions, including
move to a given position and calling a callback upon completion. A CLI also gives
access to the whole AX12 memory (see below).

To use these functions, include the headers :
```c
#include "walkingdriver/ax12driver.h"
```
For more info on AX12 driver API, see
[ax12driver.h](https://github.com/TelecomParistoc/WalkingDriver/blob/master/src/ax12driver.h).

## AX12 console utility ##

A utility is provided for configuration and diagnostic purposes. It can read and
write an AX12 memory and scan for AX12 on the bus. To use it, launch :
```
AX12
```

The serial baurate can be set with `-b` (default is 115200).

## Examples ##

Some examples are given in examples/.

To run the examples, run `make examples` from the root of the repository, and run
the programs created in examples/ (on a Raspberry Pi with IMU and toolbox module connected).
