//
//  KeyboardHardwareInterface.cpp
//  keyboard2
//
//  Created by Will Yager on 7/3/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#include "KeyboardHardwareInterface.h"
extern "C"{
#include <avr/io.h>
#include <util/delay.h>
}

void KeyboardHardwareInterface::set_red_led(bool value){
    set_red_pwm(!!value * 255);
}
void KeyboardHardwareInterface::set_green_led(bool value){
    set_green_pwm(!!value*255);
}
void KeyboardHardwareInterface::set_blue_led(bool value){
    set_blue_pwm(!!value*15);//The blue LED is much brighter
}



//LED 1
void KeyboardHardwareInterface::set_green_pwm(uint8_t value){
    if(value == 0){
        DDRB &= ~(1<<6);
        OCR1B = 0;
    }
    else{
        DDRB |= 1 << 6;
        OCR1B = value;
    }
}

//LED 2
void KeyboardHardwareInterface::set_blue_pwm(uint8_t value){
    if(value == 0){
        DDRB &= ~(1<<5);
        OCR1A = 0;
    }
    else{
        DDRB |= 1 << 5;
        OCR1A = value;
    }
}

//LED 3
void KeyboardHardwareInterface::set_red_pwm(uint8_t value){
    if(value == 0){
        DDRD &= ~(1<<7);
        OCR4D = 0;
    }
    else{
        DDRD |= 1 << 7;
        OCR4D = value;
    }
}


//These functions map a left-to-right, bottom-to-top row/column
//number to an actual pin number and then do operations on them
const unsigned char row_pin_numbers[] = {0,1,2,3,7};
const unsigned char column_pin_numbers[] = {0,1,4,5,6,7};
//Weakly pull up a row
inline void pull_up_row(unsigned char row){
    PORTB |= 1 << row_pin_numbers[row]; //DD in, value high
}
//Strongly pull down the column
inline void pull_down_column(unsigned char column){
    DDRF |= 1 << column_pin_numbers[column]; //DD out, value low
}
//Get the value of a row pin
inline bool measure_row(unsigned char row){
    return PINB & (1 << row_pin_numbers[row]);
}
//Turns off the pull-up resistors on all rows
const unsigned char row_mask = \
(1 << row_pin_numbers[0]) | \
(1 << row_pin_numbers[1]) | \
(1 << row_pin_numbers[2]) | \
(1 << row_pin_numbers[3]) | \
(1 << row_pin_numbers[4]);
const unsigned char row_off_mask = ~row_mask;
inline void reset_rows(){
    PORTB &= row_off_mask; //None of the row pins will pull up any longer.
}
//Sets all column pins to "read" mode (data direction in)
//so they can't pull down any columns
const unsigned char column_mask = \
(1 << column_pin_numbers[0]) | \
(1 << column_pin_numbers[1]) | \
(1 << column_pin_numbers[2]) | \
(1 << column_pin_numbers[3]) | \
(1 << column_pin_numbers[4]) | \
(1 << column_pin_numbers[5]);
const unsigned char column_off_mask = ~column_mask;
inline void reset_columns(){
    DDRF &= column_off_mask;
}

//Simply iterates through all PHYSICAL keys and sets its corresponding bit value to true
//if the key is pressed. Physical key numbers are resolved to digital key numbers in KeyboardComputerInterface.
BoolArray<64> KeyboardHardwareInterface::scanKeys(){
    BoolArray<64> result;
    DDRF &= column_off_mask; //Columns
    DDRB &= row_off_mask; //Rows
    PORTF &= column_off_mask;
    PORTB &= row_off_mask;
    //Step 1: Pull row up (Weak)
    //Step 2: Pull column down (Strong)
    //Step 3: Mesure row pin. If low, key is pressed.
    //Step 4: Reset all things to in, low.
#define row_size 5
#define col_size 6
    unsigned char row;
    unsigned char col;
    for(row = 0; row < row_size; row++){
        pull_up_row(row);
        for(col = 0; col < col_size; col++){
            pull_down_column(col);
            _delay_us(5); //Let signals propogate
            if(measure_row(row) == 0){ //If the row pin is low, the key is pressed
                unsigned char button_number = row*col_size + col;
                result.set(button_number, true);//Record the button as being pressed
            }//Scanned specific key
            reset_columns();
        }//Scanned all keys in row
        reset_rows();
    }//Scanned all keys
    if(!(PINB & (1<<4))) result.set(63, true);//button 3
    if(!(PIND & (1<<6))) result.set(62, true);//button 2
    if(!(PINC & (1<<7))) result.set(61,true);//button 1
    return result;
}

button_report KeyboardHardwareInterface::update(){
    button_report result;
    result.num_events = 0;
    BoolArray<64> current_raw_key_states = KeyboardHardwareInterface::scanKeys();
    for (int i=0; i<64; i++) {
        bool current_debounced_key_state = this->button_debouncer.update(i, current_raw_key_states.get(i));
        //If we've detected a change in debounced key state...
        if (current_debounced_key_state != this->recorded_button_states.get(i)) {
            this->recorded_button_states.set(i, current_debounced_key_state);

            result.events[result.num_events].button_number = i;
            result.events[result.num_events].state = current_debounced_key_state;
            result.num_events++;
            if (result.num_events == MAX_BUTTON_EVENTS_PER_SCAN) break;
        }
    }
    return result;
}