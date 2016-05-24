# Walking robot drivers #

A library providing high level access to the walking robot capabilities.
This library controls robot's modules over I2C, and provides an advanced bus
ressource sharing mecanism.

It provides (user space) drivers for :

* the IMU (BNO055)
* the toolbox module, controlling AX-12 and providing access to sensors and robot's buttons and LEDs

## Installation ##

This library is designed for Raspberry Pi with Raspbian.

* First, you need [wiringPi](http://wiringpi.com/download-and-install/).

* Once wiringPi is installed, get back the sources :
`git clone git@github.com:TelecomParistoc/WalkingDriver.git`

* cd to the root of the repository and enter `make`

* finally enter `sudo make install`

## Usage ##

Don't forget to compile (actually, link) your C/C++ app with `-lwalkingdriver` option.

### IMU driver ###

Heading access is provided by motor module driver, but access to pitch and roll
is available through imudriver.h. Include the header with :

```c
#include "robotdriver/imudriver.h"
```

See src/imudriver.h for API.

### Toolbox module driver ###

The toolbox module provides :
* AX-12 control, supporting callback on the end of moves
* 5 sensors inputs, supporting callback on change
* 5 collision detector inputs, supporting callback on change
* 3 button inputs
* 4 LEDs ouputs
* 4 PWM outputs
* voltage measurements of both batteries

To use these functions, include the header :

```c
#include "robotdriver/toolboxdriver.h"
```

For more info on toolbox module API, see [toolboxdriver.h](https://github.com/TelecomParistoc/WalkingDriver/blob/master/src/toolboxdriver.h).

## Examples ##

Some examples are given in examples/.

To run the examples, run `make examples` from the root of the repository, and run
the programs created in examples/ (on a Raspberry Pi with IMU and toolbox module connected).
