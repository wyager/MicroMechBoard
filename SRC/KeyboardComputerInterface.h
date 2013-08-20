//
//  KeyboardComputerInterface.h
//  keyboard2
//
//  Created by Will Yager on 7/3/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#ifndef __keyboard2__KeyboardComputerInterface__
#define __keyboard2__KeyboardComputerInterface__
#include <inttypes.h>
#include "keylayouts.h"
#include "Types.h"


//The following should be changed to reflect the actual physical key number/key value pairs
//So {1, KEY_A} means physical button 1 is the 'a' key
//Left keyboard
const struct key_mapping keys[] =
{
                            {61,0},{62, KEY_CAPS_LOCK},{63,KEY_NUM_LOCK},
    {24,KEY_ESC},   {25,KEY_1},{26,KEY_2},{27,KEY_3},{28,KEY_4},{29,KEY_5},
    {18,KEY_TILDE}, {19,KEY_Q},{20,KEY_W},{21,KEY_E},{22,KEY_R},{23,KEY_T},
    {12,KEY_TAB},   {13,KEY_A},{14,KEY_S},{15,KEY_D},{16,KEY_F},{17,KEY_G},
    {6,MODIFIERKEY_LEFT_SHIFT},{7,KEY_Z},{8,KEY_X},{9,KEY_C},{10,KEY_V},{11,KEY_B},
    {0,KEY_SLASH},{1,KEY_LEFT_BRACE},{2,KEY_RIGHT_BRACE},{3,MODIFIERKEY_LEFT_ALT},{4,MODIFIERKEY_LEFT_CTRL},{5,MODIFIERKEY_LEFT_GUI}
};

//Right keyboard
/*
const struct key_mapping keys[] =
{
                {61, KEY_MUTE}, {62, KEY_VOLUME_DOWN}, {63, KEY_VOLUME_UP},
    {24,KEY_6},{25,KEY_7},{26,KEY_8},{27,KEY_9},{28,KEY_0},{29,KEY_MINUS},
    {18,KEY_Y},{19,KEY_U},{20,KEY_I},{21,KEY_O},{22,KEY_P},{23,KEY_EQUAL},
    {12,KEY_H},{13,KEY_J},{14,KEY_K},{15,KEY_L},{16,KEY_SEMICOLON},{17,KEY_BACKSLASH},
    {6,KEY_N}, {7,KEY_M}, {8,KEY_COMMA},{9,KEY_PERIOD},{10,KEY_UP},{11,KEY_QUOTE},
    {0,KEY_SPACE},{1,KEY_BACKSPACE},{2,KEY_ENTER},{3,KEY_LEFT},{4,KEY_DOWN},{5,KEY_RIGHT}
};
*/

#define MAX_KEY_EVENTS_AT_ONCE 5
class KeyboardComputerInterface {
private:
    //Are we the master board (With working USB connection)
    //or slave board (sending key presses over I2C)?
    bool is_master;
    //Reflects the status of USB-spec defined LEDs
    // 1=num lock, 2=caps lock, 4=scroll lock, 8=compose, 16=kana
    unsigned char led_status;
public:
    //Return the led_status from USB if master, from memory if slave
    // 1=num lock, 2=caps lock, 4=scroll lock, 8=compose, 16=kana
    unsigned char get_led_status();
    //If we're the slave
    void set_led_status(unsigned char status);
    //The number of key events (i.e. presses or releases) we have yet to send over I2C (if we're a slave)
    unsigned char num_saved_key_events;
    //We'll record and send up to five at a time
    key_event key_events[MAX_KEY_EVENTS_AT_ONCE];
    
    KeyboardComputerInterface(bool is_master){
        this->is_master = is_master;
        this->num_saved_key_events = 0;
    }
    //takes as an argument a physical button and returns a key code
    //Returns 0 on fail (usually means failure to map physical button to key code)
    static uint16_t resolve_button_number_to_key(unsigned char button);
    //Takes a series of press/releases as described in a button report and deals with them
    void process_button_report(const button_report& report);
    //Tell the computer that a physical button has been pushed.
    //If we are a master, tell over USB. If we are a slave, tell over I2C.
    void update_key_state(uint16_t key, bool state);//Key=digital number key code (per USB spec)
    //If master, tell the computer (over USB) about the keyboard's overall state
    //Also get LED status from computer
    void inform_computer_of_keyboard_state();
};

#endif /* defined(__keyboard2__KeyboardComputerInterface__) */
