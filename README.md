# MicroMechBoard
This project is the teensy code for Will Yager's custom mechanical keyboard
project.

The code serves two functions:

1. If plugged into USB, the device will act as a stand-alone keyboard with 33
keys. ("The master")
2. If a second device is plugged into that first device, the two devices will
share relevant information over I2C. In this way, two of these devices can be
connected to form a 66-key keyboard. With some modification to the firmware and
the I2C pull-up resistors, one could theoretically chain up to 129 boards
together (I believe).

## Building/installing
1. Install `avr-gcc`.
2. Install Teensy loader.
3. `cd src`
4. `make`
5. Drag `main.hex` into Teensy loader.
6. Program Teensy.

## Modifying key layout
1. Modify `keys[]` in `KeybMapper.cpp`.
2. Replace values like `KEY_A` with other values from `keylayouts.h`.

This should be fairly simple to figure out. Within the file, I have `keys[]`
laid out just like the physical keys.

## Program structure

### main.cpp

```
       Start USB/I2C
       Determine master/slave status
       
       forever:
              led_status
              => get hardware state from HardwareController
              => debounce buttons with ButtonDebouncer
              => detect changes in buttons with ButtonDeltaDetector
              => map button changes to key changes with KeyMapper
              =: key_changes

              led_status
              => update the slave (if it exists) with the SlaveNotifier
              =: slave_key_changes

              key_changes, slave_key_changes
              => notify the master (over USB or I2C) of key changes with the MasterNotifier
              =: led_status
              

```
