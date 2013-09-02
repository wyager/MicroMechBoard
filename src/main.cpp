//
//  main.cpp
//  TeensyRNG
//
//  Created by Will Yager on 6/25/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

extern "C"{
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>
#include "usb_keyboard.h"
#include "twi.h"
}
#include <inttypes.h>
#include "KeyboardController.h"

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))


volatile bool i2c_master_has_been_found = false;

//I think this is necessary, since we can't pass an argument to the interrupt handlers
//And we only create the KeyboardController after doing some other stuff
KeyboardController* keyboard_pointer;

//Called when the master sends us some data
//The master sends us data either to A)inform us that we are to be a slave
//or B)inform us of data coming from the host (like the state of various
//LEDs (cap lock etc.) that we can choose to act on.
void interrupt_hander_to_read_i2c_message_from_master(uint8_t* buf,int numBytes);
//Called when the master asks for some data (i.e. keyboard status)
void interrupt_handler_to_respond_to_request_from_I2C_master(void);
//Set up hardware
void hardware_init();

int main(void)
{
	// set for 16 MHz clock
	CPU_PRESCALE(0);
    hardware_init();

	usb_init();//Turn on USB hardware
    
    twi_setAddress(SLAVE_KEYBOARD_ADDR);//Turn on I2C device as a slave board
    twi_attachSlaveTxEvent(interrupt_handler_to_respond_to_request_from_I2C_master);
    twi_attachSlaveRxEvent(interrupt_hander_to_read_i2c_message_from_master);
    twi_init();
    
    
    //wait until:
    //   USB connects successfully (we are the master board)
    //   OR
    //   We get an init packet over I2C (we are the slave board)
	while (!(usb_configured() || i2c_master_has_been_found)){}


    bool is_master = usb_configured();//If we successfully configured USB, we are the master
    //Otherwise, we are the slave

    KeyboardController keyboard(is_master); //Create a keyboard controller object that deals with
    //A)Scanning physical keys and resolving them to USB codes (via KeyboardHardwareInterface)
    //B)Asking the slave if it has any key-related updates (it handles this internally)
    //    (But only if this board is running as a master board)
    //C)Using KeyboardComputerInterface to either:
    //    C1)Notify the computer of key presses via USB (if master board)
    //    C2)Notify the master board of key presses via I2C (if slave board)
    if(!is_master){//We are a slave. Prepare for slavery
        //Disable USB hardware to save power
        UDCON = 1<<DETACH; //Detach data pins
        USBCON = 1<<FRZCLK; //disable USB controller
        PLLCSR &= ~(1<<PLLE); //disable PLL
        UHWCON &= ~(1<<UVREGE);//Disable USB power regulator

        //Tell the interrupt handler where to look for stored keypress data
        keyboard_pointer = &keyboard;

    }
    if(is_master){//We are a master. Prepare for mastery
        twi_setAddress(0);
    }
    
	_delay_ms(1000);
	
	while (1) {
		if(is_master){
            //////Super duper ultra mega master function/////
            /////Does all the main keyboard-related stuff////
            keyboard.update();
            //////////If we're the master, update()//////////
            ////////////polls the slave over I2C/////////////
            /////////////////////////////////////////////////
        }
        //You may be wondering: If we're not a master, do we ever do anything?
        //The answer, young grasshopper, is yes. However, all actions are initiated
        //by the master over I2C. See interupt handlers defined above
	}
}


void hardware_init(){
    //Do NOT use Teensy's LED, as PD6 is currently used for
    //measuring a key and will burn out on key press if
    //the LED is turned on
    
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


void interrupt_hander_to_read_i2c_message_from_master(uint8_t* buf,int numBytes){
    if(!numBytes) return;
    
    if(buf[0] == 'S'){//If this is a valid init packet from the master
        i2c_master_has_been_found = true;
    }
    else if(buf[0] == 'L' && numBytes >= 2){//LED status update
        if(keyboard_pointer){//Make sure this pointer is set!
            unsigned char led_status = buf[1];
            keyboard_pointer->computer_interface.set_led_status(led_status);
        }
    }
}


void interrupt_handler_to_respond_to_request_from_I2C_master(void){
    if(!keyboard_pointer){//I hope this never happens
        return;
    }

    uint8_t tx_buffer[16];
    
    //Grab the most recent recorded key events (if any) and send them over I2C
    uint8_t num_events = keyboard_pointer->computer_interface.num_saved_key_events;
    
    tx_buffer[0] = num_events;
    
    for (uint8_t event_number = 0; event_number < num_events; event_number++) {
        key_event event = keyboard_pointer->computer_interface.key_events[event_number];
        tx_buffer[event_number*3+1] = event.key & 0xFF;
        tx_buffer[event_number*3+2] = (event.key >> 8) & 0xFF;
        tx_buffer[event_number*3+3] = event.state;
    }
    
    keyboard_pointer->computer_interface.num_saved_key_events = 0;
    
    uint8_t bytes_used = 1+num_events*3;
    
    for (uint8_t i = bytes_used; i<16; i++) {
        tx_buffer[i]=0;
    }

    twi_transmit(tx_buffer, 16);//returns 1 on too long, 2 on not slave, 0 success
    
    //Refresh key status
    keyboard_pointer->update();
}