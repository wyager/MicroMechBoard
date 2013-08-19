//
//  KeyboardController.cpp
//  keyboard2
//
//  Created by Will Yager on 6/18/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#include "KeyboardController.h"
#include "KeyboardHardwareInterface.h"
#include <inttypes.h>
extern "C"{
#include "twi.h"
}
//Returns true if slave keyboard found, false otherwise
bool check_for_slave_keyboard_on_i2c_bus(){
    uint8_t packet[] = {'S'};
    uint8_t result = twi_writeTo(SLAVE_KEYBOARD_ADDR, packet, 1, 1, 1);
    if (result == 0) return true;//success
    return false;//failure
}


void KeyboardController::update(){
    //Master or slave, scan our own hardware
    BoolArray<64> current_raw_key_states = KeyboardHardwareInterface::scanKeys();
    for (int i=0; i<64; i++) {
        signed char current_debounced_key_state = this->button_debouncer.update(i, current_raw_key_states.get(i));
        //If we've detected a change in debounced key state...
        if (current_debounced_key_state != this->recorded_button_states.get(i)) {
            this->recorded_button_states.set(i, current_debounced_key_state);
            //Tell the computer (or master) that the key has been pressed or depressed
            this->computer_interface.update_button_state(i, current_debounced_key_state);
        }
    }

    //If no slave found yet:
    //   Check for slave
    //If slave found already:
    //   Ask slave to send key state changes over I2C (e.g. "USB spec spacebar has been pressed")
    //   Tell PC over USB what the slave just told us
    //   Slave will then scan its own hardware
    if(this->is_master){
        if(!this->slave_keyboard_has_been_found){
            slave_keyboard_has_been_found = check_for_slave_keyboard_on_i2c_bus();
        }
        if(this->slave_keyboard_has_been_found){
            uint8_t packet[16];
            twi_readFrom(SLAVE_KEYBOARD_ADDR, packet, 16, 1);
             //1 byte length. Up to 5x 3 byte key event messages.
            uint8_t number_of_messages_being_sent = packet[0];
            for(unsigned char i=0; i<number_of_messages_being_sent; i++){ //Recieve 1 key event (3 bytes) at a time
                uint16_t key = 0; //Key code from USB spec
                char state; //pressed = 1, released = 0
                key |= packet[i*3+1];
                key |= packet[i*3+2] << 8;
                state = packet[i*3+3];
                this->computer_interface.update_key_state(key, state);
            }//Done recieving up to 5 key events
        }//Done doing master key press recieve stuff
    }
    
    if(this->is_master){
        this->computer_interface.inform_computer_of_keyboard_state();
    }
    
    //Do stuff with keyboard LEDs
    
    //Tell slave the LED status
    if(this->is_master){
        if(this->slave_keyboard_has_been_found){
            uint8_t packet[2] = {'L',0};
            packet[1] = this->computer_interface.get_led_status();
            twi_writeTo(SLAVE_KEYBOARD_ADDR, packet, 2, 1, 1);
        }
    }
    unsigned char led_status = this->computer_interface.get_led_status();
    KeyboardHardwareInterface::set_green_led(this->is_master);//If we're the master board
    KeyboardHardwareInterface::set_blue_led(led_status & 2);//caps lock
    KeyboardHardwareInterface::set_red_led(led_status & 1);//num lock
}

