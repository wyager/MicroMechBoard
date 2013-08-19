//
//  KeyboardHardwareInterface.h
//  keyboard2
//
//  Created by Will Yager on 7/3/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#ifndef __keyboard2__KeyboardHardwareInterface__
#define __keyboard2__KeyboardHardwareInterface__

#include "BoolArray.h"

class KeyboardHardwareInterface {
    
public:
    static void set_red_led(bool value);
    static void set_green_led(bool value);
    static void set_blue_led(bool value);
    //Simply iterates through all PHYSICAL keys and sets its corresponding bit value to true
    //if the key is pressed. Physical key numbers are resolved to digital key numbers in KeyboardComputerInterface.
    static BoolArray<64> scanKeys();
   
};

#endif /* defined(__keyboard2__KeyboardHardwareInterface__) */
