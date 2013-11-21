//  KeyMapper.cpp
//  Created by Will Yager on 7/3/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//  Maps physical buttons to USB spec key codes. 

#include "KeyMapper.h"
#include "keylayouts.h"

//Ties a physical button number to a USB key spec number
struct key_mapping {
    //Physical button
    uint8_t button;
    //USB key data
    uint16_t key;
};

//The following should be changed to reflect the actual physical key number/key value pairs
//So {1, KEY_A} means physical button 1 is the 'a' key. 
//The following layouts mimic their physical layout. The button number is, in cartesian coordinates, x+6y

//Left keyboard 
/*
const struct key_mapping keys[] =
{
                            {61,0},{62, KEY_CAPS_LOCK},{63,KEY_NUM_LOCK},
    {24,KEY_ESC},   {25,KEY_1},{26,KEY_2},{27,KEY_3},{28,KEY_4},{29,KEY_5},
    {18,KEY_TILDE}, {19,KEY_Q},{20,KEY_W},{21,KEY_E},{22,KEY_R},{23,KEY_T},
    {12,KEY_TAB},   {13,KEY_A},{14,KEY_S},{15,KEY_D},{16,KEY_F},{17,KEY_G},
    {6,MODIFIERKEY_LEFT_SHIFT},{7,KEY_Z},{8,KEY_X},{9,KEY_C},{10,KEY_V},{11,KEY_B},
    {0,KEY_SLASH},{1,KEY_LEFT_BRACE},{2,KEY_RIGHT_BRACE},{3,MODIFIERKEY_LEFT_ALT},{4,MODIFIERKEY_LEFT_CTRL},{5,MODIFIERKEY_LEFT_GUI}
};
*/
//Right keyboard

const struct key_mapping keys[] =
{
                {61, KEY_MUTE}, {62, KEY_VOLUME_DOWN}, {63, KEY_VOLUME_UP},
    {24,KEY_6},{25,KEY_7},{26,KEY_8},{27,KEY_9},{28,KEY_0},{29,KEY_MINUS},
    {18,KEY_Y},{19,KEY_U},{20,KEY_I},{21,KEY_O},{22,KEY_P},{23,KEY_EQUAL},
    {12,KEY_H},{13,KEY_J},{14,KEY_K},{15,KEY_L},{16,KEY_SEMICOLON},{17,KEY_BACKSLASH},
    {6,KEY_N}, {7,KEY_M}, {8,KEY_COMMA},{9,KEY_PERIOD},{10,KEY_UP},{11,KEY_QUOTE},
    {0,KEY_SPACE},{1,KEY_BACKSPACE},{2,KEY_ENTER},{3,KEY_LEFT},{4,KEY_DOWN},{5,KEY_RIGHT}
};


//Physical button number => Virtual button code
uint16_t KeyMapper::map_button_to_key(uint8_t button){
    for (uint8_t i = 0; i < (sizeof(keys)/sizeof(key_mapping)); ++i)
    {
        if(keys[i].button == button){
            return keys[i].key;
        }
    }
    return 0; //No button assignment found
}

//Takes a lot of button-number-indexed deltas and puts the relevant 
//key-code-associated delta in the first free space of the result
KeysDelta KeyMapper::resolve(const ButtonsDelta& button_changes){
    KeysDelta result;
    uint8_t count = 0;
    for (int i = 0; i < 64; ++i)
    {
        result.deltas[i].delta = 0;
        if (button_changes.deltas[i] != 0)
        {
            result.deltas[count].delta = button_changes.deltas[i];
            result.deltas[count].key   = this->map_button_to_key(i);
            count++;
        }
    }
    return result;
}