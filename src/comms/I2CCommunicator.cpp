//  I2CCommunicator.cpp
//  Created by William Yager on 10/10/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//  Communicates over I2C

#include "comms/I2CCommunicator.h"
extern "C"{
#include "device/twi.h"
}

/*********** All nasty I2C stuff is mediated in this file. ***********
 ********* Data from I2C is put in these here static values. ********/
 //Called when the master sends us some data
 //The master sends us data either to A)inform us that we are to be a slave
 //or B)inform us of data coming from the host (like the state of various
 //LEDs (cap lock etc.)) that we can choose to act on.
 void interrupt_hander_to_read_i2c_message_from_master(uint8_t* buf,int numBytes);
 //Called when the master asks for some data (i.e. keyboard status)
 void interrupt_handler_to_respond_to_request_from_I2C_master(void);
 //Changed to true if we find an I2C master
 volatile bool i2c_master_has_been_found = false;
 //The USB LED indicator value;
 volatile uint8_t LED_status = 0;
 //The packet the slave wants to send over I2C
Packet outbound_packet;
//Set to true when something is to be sent. Set to false when it is sent.
volatile bool outbox_full = false;

void interrupt_hander_to_read_i2c_message_from_master(uint8_t* buf,int numBytes){
    if (numBytes != 16) { //Invalid packet
        return;
    }

    if (buf[0] == 'S') { //Slave init packet
        i2c_master_has_been_found = true;;
        return;
    }

    if (buf[0] == 'L'){ //LED status packet
        LED_status = buf[1];
    }
}

void interrupt_handler_to_respond_to_request_from_I2C_master(void){
    if(outbox_full){
        twi_transmit(outbound_packet.data, 16);
    } else {
        uint8_t zeros[16] = {0};
        twi_transmit(zeros, 16);
    }
    outbox_full = false;
}
/************************ Back to nice C++ **************************/

I2CCommunicator::I2CCommunicator(){
    this->_is_master = false;
    twi_setAddress(1);//Slave address
    twi_attachSlaveTxEvent(interrupt_handler_to_respond_to_request_from_I2C_master);
    twi_attachSlaveRxEvent(interrupt_hander_to_read_i2c_message_from_master);
    twi_init();
}

void I2CCommunicator::become_master(){
    twi_setAddress(0);//Master address
    this->_is_master = true;
    //If we get a response, we have a slave. Otherwise, we don't.
    this->has_slave = this->send_packet(SlaveInitPacket());
}

bool I2CCommunicator::send_packet(const Packet& packet){
    if(this->_is_master){
        //Slave address, packet pointer, length, wait, send stop bit
        uint8_t result = twi_writeTo(1, packet.data, 16, true, true);
        if(result == 0) return true; //Successful message send to slave
    } else {
        outbound_packet = packet; //Put the data in the tx buffer
        outbox_full = true; //Tell the interrupt to send our data
        while(outbox_full){}; //Wait for the interrupt to send our data
    }
    return false; //Either we are a slave or the message to the slave failed
}

//Only does anything if we're the master. Asks for a packet from the slave.
//The slave can send whatever it wants, but a properly programmed slave
//will only send key-press packets. 
Packet I2CCommunicator::request_packet(){
    Packet result;
    //Slave address, buffer pointer, length, send stop
    twi_readFrom(1, result.data, 16, true);
    return result;
}

bool I2CCommunicator::is_master(){
    return this->_is_master;
}

bool I2CCommunicator::is_slave(){
    return i2c_master_has_been_found;
}

uint8_t I2CCommunicator::update(const KeysDelta& key_changes){
    KeyPacket events;
    uint8_t i = 0;
    while(key_changes.deltas[i].delta != 0){
        events.add_KeyDelta(key_changes.deltas[i]);
        i++;
        if(events.is_full()){
            this->send_packet(events);
            events.data[0] = 0; //"Emtpy" the packet
        }
    }
    if(!events.is_empty()) this->send_packet(events);
    return LED_status;
}


Packet::Packet(){
    for (uint8_t i=0; i<16; i++) {
        this->data[i] = 0;
    }
}
Packet::Packet(const uint8_t* data){
    for (uint8_t i=0; i<16; i++) {
        this->data[i] = data[i];
    }
}
bool Packet::is_SlaveInitPacket(){return this->data[0] == 'S';}
bool Packet::is_LEDPacket(){return this->data[0] == 'L';}
bool Packet::is_KeyPacket(){return this->data[0] <= 5;}



SlaveInitPacket::SlaveInitPacket(){
    this->data[0] = 'S';
}


LEDPacket::LEDPacket(uint8_t led_status){
    this->data[0] = 'L';
    this->data[1] = led_status;
}


KeyPacket::KeyPacket(){
    this->data[0] = 0; //Length
}
KeyPacket::KeyPacket(const Packet& packet){
    for (uint8_t i=0; i<16; i++) {
        this->data[i] = packet.data[i];
    }
}
bool KeyPacket::is_full(){return (this->count() >= 5);}
bool KeyPacket::is_empty(){return (this->count() == 0);}
uint8_t KeyPacket::count(){return this->data[0];}
//Returns true if there's still space.
bool KeyPacket::add_KeyDelta(KeyDelta delta){
    if (!this->is_full()) {
        uint8_t index = this->data[0]*3 + 1;
        this->data[index]   = delta.key >> 8;
        this->data[index+1] = delta.key &  0xFF;
        this->data[index+2] = delta.delta;
        this->data[0]++;
    }
    return !this->is_full();
}
//Removes and returns the last KeyDelta in the packet.
KeyDelta KeyPacket::pop_KeyDelta(){
    KeyDelta result;
    if(!this->is_empty()){
        uint8_t index = this->data[0]*3 + -2;
        result.key  |= this->data[index] << 8;
        result.key  |= this->data[index+1];
        result.delta = this->data[index+2];
        this->data[0]--;
    }
    return result;
}
