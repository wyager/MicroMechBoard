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
2. Install `teensy loader`.
3. `cd src`
4. `./make`
5. Drag `main.hex` into Teensy loader.
6. Program Teensy.

## Modifying key layout
1. Modify `keys[]` in `KeyboardComputerInterface.h`.
2. Replace values like `KEY_A` with other values from `keylayouts.h`.

This should be fairly simple to figure out. Within the file, I have `keys[]`
laid out just like the physical keys.

## Program structure

### main.cpp

```
       Start USB/I2C
       Determine master/slave status
       Set up a KeyboardController in master/slave mode
       if master:
              Start I2C master mode
              Periodically do KeyboardControler::update()
       if slave:
              Disable USB hardware
              Set up a listener for requests for I2C data
              Upon I2C request:
                     Do KeyboardController::update()
                     Send any key press/release events to master over I2C
```

### KeyboardController.cpp

```
       Tie together Hardware, I2C, and USB
       Spawns a KeyboardHardwareInterface to monitor physical buttons
       Spawns a DebouncerArray to debounce physical buttons
       Spawns a KeyboardComputerInterface to:
              Send keystrokes over USB (master)
              Send keystrokes over I2C (slave)
       if master:
              Detects slave keyboards
              Scans its own hardware for key presses
              Scans slave hardware for key presses (over I2C)
              Tells the slave about caps/num/etc. lock status
              Send all key presses over USB
       if slave:
              Wait for request from master
              Upon such request:
                     Send relevant info to master
                     Scan its own hardware after sending info
       Manages LEDs and such similar tasks
```

### KeyboardHardwareInterface.cpp

```
       Scans the key matrix
       Builds a compact bool array representing all key states:
              key_x = count right from bottom left
              key_y = count up from bottom left
              key_index = key_x + key_y * x_max
              key_values[key_index] = scanned_values[key_index]
       Returns this bool array for debouncing by the KeyboardController
       Provides convenience functions for LEDs and such
```

### KeyboardComputerInterface.cpp

```
       Resolves physical button indexes to USB spec key values:
              Which button = which key is determined in the header file
       if master:
              Use the PJRC keyboard library press() and release() functions
              to send keystrokes over USB
       if slave:
              Save up to 5 key press/release events in an array
              The slave I2C request handler will then send these
                     press/release events to the master over I2C
```
