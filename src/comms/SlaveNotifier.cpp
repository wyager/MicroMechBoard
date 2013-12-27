//  SlaveNotifier.cpp
//  Created by William Yager on 10/12/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.

#include "comms/SlaveNotifier.h"

KeysDelta SlaveNotifier::update(uint8_t led_status){

    KeysDelta result = {}; //Initialize to 0

    //No slave. Just return the empty KeysDelta
	if(!i2c_communicator.is_master()) return result;
    
    //Get all the key events from the slave and store them
    uint8_t event_count = 0;
    for(;;){
        //(Try to) get a packet from the slave over I2C
        KeyPacket recieved = (KeyPacket)i2c_communicator.request_packet();
        //Return if the packet isn't what we expect
        if (!recieved.is_KeyPacket()) break;
        //Copy all the KeyDeltas from the packet
        uint8_t count = recieved.count();
        for(uint8_t i=0; i<count; i++){
            result.deltas[event_count] = recieved.pop_KeyDelta();
            event_count++;
            if(event_count == 64) break; //We're full. Should never happen
        }
        //The slave is done sending us key events
        if(count != 5) break;
    }
    
    
    if(this->old_led_status != led_status){
        i2c_communicator.send_packet(LEDPacket(led_status));
        this->old_led_status = led_status;
    }

    return result;
}