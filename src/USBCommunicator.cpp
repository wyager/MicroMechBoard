//  USBCommunicator.cpp
//  Created by William Yager on 10/12/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//  Responsible for communicating with the host PC (if one is attached)

#include "USBCommunicator.h"
extern "C"{
#include "usb_keyboard.h"
#include <avr/io.h>
}

USBCommunicator::USBCommunicator(){
    usb_init(); //Turn on USB hardware
}

//Sends all deltas in key_changes and slave_key_changes to the host PC.
//Assumes the non-zero deltas are packed in the lowest indices
uint8_t USBCommunicator::update(const KeysDelta& key_changes, const KeysDelta& slave_key_changes){
    uint8_t i = 0;
    while(key_changes.deltas[i].delta != 0){ 
        this->send_key_event(key_changes.deltas[i]);
        i++;
    }
    i = 0;
    while(slave_key_changes.deltas[i].delta != 0){
        this->send_key_event(slave_key_changes.deltas[i]);
        i++;
    }
    usb_keyboard_send();
    return keyboard_leds;
}

//Returns true if a host PC is attached
bool USBCommunicator::is_connected(){
    return usb_configured() != 0;
}

void USBCommunicator::send_key_event(const KeyDelta& event){
    if(event.delta > 0) usb_press(event.key);
    if(event.delta < 0) usb_release(event.key);
}


//Disable USB hardware to save power. Can be used if we're not connected via USB
void USBCommunicator::turn_off(){
    UDCON = 1<<DETACH; //Detach data pins
    USBCON = 1<<FRZCLK; //disable USB controller
    PLLCSR &= ~(1<<PLLE); //disable PLL
    UHWCON &= ~(1<<UVREGE);//Disable USB power regulator
}