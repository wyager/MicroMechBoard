//  HardwareController.cpp
//  Created by Will Yager on 7/3/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//  Controls hardware peripherals and sets the LEDs

#include "device/HardwareController.h"
extern "C"{
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
}

//For nifty LED fade effects
const uint8_t PROGMEM half_sin_curve[] = {0, 8, 16, 23, 31, 39, 47, 55, 63, 71,\
 78, 86, 93, 101, 108, 115, 122, 129, 136, 143, 149, 156, 162, 168,\
  174, 180, 185, 191, 196, 201, 206, 210, 215, 219, 223, 227, 230,\
   234, 237, 239, 242, 244, 246, 248, 250, 251, 252, 253, 254, 254,\
    255, 254, 254, 253, 252, 251, 250, 248, 246, 244, 242, 239, 237,\
     234, 230, 227, 223, 219, 215, 210, 206, 201, 196, 191, 185, 180,\
      174, 168, 162, 156, 149, 143, 136, 129, 122, 115, 108, 101, 93, \
       86, 78, 71, 63, 55, 47, 39, 31, 23, 16, 8, 0};

void HardwareController::set_red_led(bool value){
    set_red_pwm(!!value * 255);
}
void HardwareController::set_green_led(bool value){
    set_green_pwm(!!value*255);
}
void HardwareController::set_blue_led(bool value){
    set_blue_pwm(!!value*15);//The blue LED is much brighter
}



//LED 1
void HardwareController::set_green_pwm(uint8_t value){
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
void HardwareController::set_blue_pwm(uint8_t value){
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
void HardwareController::set_red_pwm(uint8_t value){
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
//if the key is pressed.
ButtonsState HardwareController::update(uint8_t led_state){
	HardwareController::set_green_led(led_state & 1);//Num lock
	HardwareController::set_blue_led(led_state & 2);//Caps lock
    
    //Pull up the 3 function button pins and check if they are pulled down
    PORTB |= (1<<4);
    PORTC |= (1<<7);
    PORTD |= (1<<6);

    ButtonsState result = {};
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
                result.states[button_number] = true;//Record the button as being pressed
            }//Scanned specific key
            reset_columns();
        }//Scanned all keys in row
        reset_rows();
    }//Scanned all keys
    
    if(!(PINB & (1<<4))) result.states[63] = true;//button 3
    if(!(PIND & (1<<6))) result.states[62] = true;//button 2
    if(!(PINC & (1<<7))) result.states[61] = true;//button 1
    //Now pull them back down
    PORTB &= ~(1<<4);
    PORTC &= ~(1<<7);
    PORTD &= ~(1<<6);
    return result;
}

HardwareController::HardwareController(){
    //Do NOT use Teensy's LED, as PD6 is currently used for
    //measuring a key. Pushing the key will short the pin,
    //which will burn out the pin if it is fully turned
    //on. The LED pin being weakly pulled up is fine.
    
    //Columns (0 to 5, left to right):
    //   PORTF: 0, 1, 4, 5, 6, 7
    DDRF = 0;
    PORTF = 0;
    
    //Rows (0 to 4, bottom to top):
    //    PORTB: 7, 3, 2, 1, 0
    DDRB = 0;//Rows
    PORTB = 0;
    
    //Turn on I2C internal pullups. Not really necessary, since we have external pullups too
    PORTD |= (1<<0) | (1<<1);
    
    
    //Key 1: PB4
    PORTB |= 1 << 4;//Enable pullup resistor for key 1
    //Key 2: PD6
    PORTD |= 1 << 6;//Enable pullup resistor for key 2
    //Key 3: PC7
    PORTC |= 1 << 7;//Enable pullup resistor for key 3
    
    //LED 1&2 setup
    TCCR1B = 1 << CS10; //Use the full 16MHz clock as a PWM clock source for timer 1.
    //For fast PWM, 8 bit, we want to set WGMn2 and WGMn0.
    TCCR1A = 1 << WGM10;
    TCCR1B |= 1 << WGM12;
    //For fast PWM: to clear on compare match with OCRnx and set at TOP, set COMnx1,
    //where n is timer number and x is channel leter (A/B/C)
    //Blue led is OC1A (timer 1, channel A)
    TCCR1A |= 1 << COM1A1;
    TCCR1A |= 1 << COM1B1;
    //LED 2 is OCR1A
    //LED 1 is OCR1B
    
    //LED 3 setup
    //Use full 16MHz clock source for timer 4
    TCCR4B = 1 << CS40;
    //To enable fast PWM on timer 4, we have to set PWM4x to 1
    TCCR4C = 1 << PWM4D;
    //Set on match, clear on 0
    TCCR4C |= 1 << COM4D1;
    //Led 3 is OCR4D
}


