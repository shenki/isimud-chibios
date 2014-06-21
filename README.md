Isimud HAB Payload Computer
===========================

This is the next generation High Altitude Balloon payload computer designed to
replace the NUT Arduino based system used by Project Horus. It is based upon
ChibiOS/RT and runs on a STM32 ARM Cortex-M0 based platform.

Isimud is the messenger of the god Enki in Sumerian mythology.

Building
--------

We use a gcc 4.8.2 based cross compiler to build, and OpenOCD combined with gdb
to load and debug the firmware over the ST-Link debug adaptor on the
STM32F0DISCOVERY. Note that OpenOCD 0.8 may be required for correct ST-Link
support.

The versions included in Ubuntu Utopic (14.10) are sufficient.

```
$ sudo apt-get install gcc-arm-none-eabi gdb-arm-none-eabi openocd git
$ git clone https://github.com/shenki/isimud-chibios
$ cd isimud-chibios
$ make
$ openocd
```

In a second terminal, from the isimud-chibios directory:

```
$ arm-none-eabi-gdb
(gdb) c
```

Licence
-------

Isimud is licenced GPLv3.

ChibiOS/RT is licenced under a mixture of the Apache 2.0 licence and GPLv3,
with the exception of some STM32 headers from ST Microelectronics which are
licenced for use with the microcontroller.
