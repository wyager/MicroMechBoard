//  MasterNotifier.h
//  Created by Will Yager on 7/3/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//  Talks to the master over USB or I2C, whichever it is attached by.

#ifndef __keyboard2__MasterNotifier__
#define __keyboard2__MasterNotifier__

#include "Types.h"
#include "USBCommunicator.h"
#include "I2CCommunicator.h"

class MasterNotifier {
private:
    bool is_master;
    USBCommunicator& usb_communicator;
    I2CCommunicator& i2c_communicator;
public:
	MasterNotifier(USBCommunicator& usb, I2CCommunicator& i2c);
	uint8_t notify(const KeysDelta& key_changes, const KeysDelta& slave_key_changes);
};

#endif /* defined(__keyboard2__MasterNotifier__) */
