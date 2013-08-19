//
//  KeyboardComputerInterface.cpp
//  keyboard2
//
//  Created by Will Yager on 7/3/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#include "KeyboardComputerInterface.h"
extern "C"{
#include "usb_keyboard.h"
#include <avr/interrupt.h>
}

//"Button" is a number for a physical button
//"Key" is a number for the USB spec
//This function takes a button and returns the corresponding key
//Returns 0 on failure to map button to key
uint16_t KeyboardComputerInterface::resolve_button_number_to_key(unsigned char button){
    for (unsigned int i = 0; i<(sizeof(keys)/sizeof(key_mapping)); i++) {
        if(keys[i].button == button){
            return keys[i].key;
        }
    }
    return 0;
}

//Send keyboard info using teensy's keyboard send command
void KeyboardComputerInterface::inform_computer_of_keyboard_state(){
    if (this->is_master) {
        usb_keyboard_send();
    }
}

//Reflects the status of USB-spec defined LEDs
// 1=num lock, 2=caps lock, 4=scroll lock, 8=compose, 16=kana
unsigned char KeyboardComputerInterface::get_led_status(){
    unsigned char status = 0;
    if(this->is_master){
        status = keyboard_leds;
    }
    else{
        status = this->led_status;
    }
    return status;
}

//If we're not actually connected over USB, this function is called
//when the master tells us the LED status over I2C
void KeyboardComputerInterface::set_led_status(unsigned char status){
    this->led_status = status;
}

void KeyboardComputerInterface::update_button_state(unsigned char button, char state){
    uint16_t keycode = KeyboardComputerInterface::resolve_button_number_to_key(button);
    if (keycode != 0){
        this->update_key_state(keycode, state);
    }
}

void KeyboardComputerInterface::update_key_state(uint16_t key, char state){
    if(this->is_master){//Tell USB library about key state
        if(state){
            usb_press(key);
        }
        if(!state){
            usb_release(key);
        }
    }
    else{
        //If slave: put any new key press/release events in an array.
        //Next time the master asks for an update over I2C, the interrupt
        //handler will send all key events over I2C and reset the number
        //of saved key events back to 0.
        if(this->num_saved_key_events < MAX_KEY_EVENTS_AT_ONCE){//If we still have room to save a key event
            key_event event_to_send_over_i2c;
            event_to_send_over_i2c.key = key;
            event_to_send_over_i2c.state = state;
            this->key_events[num_saved_key_events] = event_to_send_over_i2c;
            this->num_saved_key_events++;
        }
    }
}


