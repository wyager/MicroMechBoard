//  I2CCommunicator.h
//  Created by William Yager on 10/10/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//  Does the talking over I2C. 
//  This file also specifies some I2C packet types.

#ifndef ____I2CCommunicator__
#define ____I2CCommunicator__

#include "Types.h"
//Used for sending messages over I2C
class Packet{
public:
    uint8_t data[16];
    Packet();
    Packet(const uint8_t* data);
    bool is_SlaveInitPacket();
    bool is_LEDPacket();
    bool is_KeyPacket();
};

class SlaveInitPacket : public Packet{
public:
    SlaveInitPacket();
};

class LEDPacket : public Packet{
public:
    LEDPacket(uint8_t led_status);
};

class KeyPacket : public Packet{
public:
    KeyPacket();
    KeyPacket(const Packet& packet);
    bool is_full();
    bool is_empty();
    uint8_t count();
    //Returns true if there's still space
    bool add_KeyDelta(KeyDelta delta);
    KeyDelta pop_KeyDelta();
};


class I2CCommunicator {
private:
    bool _is_master;
    bool has_slave;
public:
    //Starts I2C in slave mode.
    I2CCommunicator();
    //Assumes master status
    void become_master();
    //Sends a packet (to the master or slave)
    //Returns true if this->is_master and slave got the packet, false otherwise
    //If we're the slave, this blocks until the master recieves the packet
    bool send_packet(const Packet& packet);
    //Requests a packet (from the slave)
    Packet request_packet();
    //Returns true if we are the I2C master
    bool is_master();
    //Send any key changes to the master. Returns the LED status.
    uint8_t update(const KeysDelta& key_changes);
    //Returns true if we are a slave
    bool is_slave();
};

#endif /* defined(____I2CCommunicator__) */
