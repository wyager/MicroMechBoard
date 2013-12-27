//  SlaveNotifier.h
//  Created by William Yager on 10/12/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//  Talks to the slave (if it exists) over I2C

#ifndef ____SlaveNotifier__
#define ____SlaveNotifier__

#include "Types.h"
#include "comms/I2CCommunicator.h"

class SlaveNotifier {
    I2CCommunicator& i2c_communicator;
    uint8_t old_led_status;
public:
    SlaveNotifier(I2CCommunicator& i2c):i2c_communicator(i2c){};
    KeysDelta update(uint8_t led_status);
};

#endif /* defined(____SlaveNotifier__) */
