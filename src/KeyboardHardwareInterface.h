//
//  KeyboardHardwareInterface.h
//  keyboard2
//
//  Created by Will Yager on 7/3/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#ifndef __keyboard2__KeyboardHardwareInterface__
#define __keyboard2__KeyboardHardwareInterface__

#include "DebouncerArray.h"
#include "BoolArray.h"
#include <inttypes.h>
#include "Types.h"




class KeyboardHardwareInterface {
private:
	//This object prevents us from accidentally thinking that the key was pressed/depressed lots of times
    //when really the key is just bouncing up and down microscopically.
    DebouncerArray<64> button_debouncer;
    //This records what we *think* the button states are, so we know when we need to record a change.
    BoolArray<64> recorded_button_states;
public:
    static void set_red_led(bool value);
    static void set_green_led(bool value);
    static void set_blue_led(bool value);
    static void set_red_pwm(uint8_t value);
    static void set_green_pwm(uint8_t value);
    static void set_blue_pwm(uint8_t value);
    //Simply iterates through all PHYSICAL keys and sets its corresponding bit value to true
    //if the key is pressed. Physical key numbers are resolved to digital key numbers in KeyboardComputerInterface.
    static BoolArray<64> scanKeys();

    //Inititalize the debouncer with x level of sensitivity.
    KeyboardHardwareInterface():button_debouncer(5){}
    
    //scan keys and return a button_report with any important updates.
    button_report update();
};

#endif /* defined(__keyboard2__KeyboardHardwareInterface__) */
