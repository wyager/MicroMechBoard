//
//  KeyboardController.h
//  keyboard2
//
//  Created by Will Yager on 6/18/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#ifndef __keyboard2__KeyboardController__
#define __keyboard2__KeyboardController__

#include "BoolArray.h"
#include "KeyboardComputerInterface.h"
#include "KeyboardHardwareInterface.h"
#include <inttypes.h>




class KeyboardController {
private:
    bool is_master;//Are we master board (USB connection) or slave board (I2C only)?
    bool slave_keyboard_has_been_found;//If we're the master, have we found a slave yet?
public:
    //Initialize the computer interface with the knowledge of whether to operate as
    //a master or slave
    KeyboardController(bool is_master):computer_interface(is_master){
        this->is_master = is_master;
        this->slave_keyboard_has_been_found = false;
    }
    //update() is the heartbeat of this class. It scans the keyboard hardware and looks for changes, and
    //then acts on them (by telling the keyboard-computer interface object about presses/releases)
    void update();
    
    //This is the interface between what keys we know are physically pressed and how we tell the computer this.
    //The KeyboardComputerInterface also converts physical key numbers into USB HID keyboard spec key numbers
    KeyboardComputerInterface computer_interface;
    KeyboardHardwareInterface hardware_interface;
};

#endif /* defined(__keyboard2__KeyboardController__) */
