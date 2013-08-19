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
    if(value){
        PORTD |= 1<<7;
    }
    else{
        PORTD &= ~(1<<7);
    }
}
void KeyboardHardwareInterface::set_green_led(bool value){
    if(value){
        PORTB |= 1<<6;
    }
    else{
        PORTB &= ~(1<<6);
    }
}
void KeyboardHardwareInterface::set_blue_led(bool value){
    if(value){
        PORTB |= 1<<5;
    }
    else{
        PORTB &= ~(1<<5);
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
    if(!(PINB & (1<<4))) result.set(61, true);//button 1
    if(!(PIND & (1<<6))) result.set(62, true);//button 2
    if(!(PINC & (1<<7))) result.set(63,true);//button 3
    return result;
}