//
//  BoolArray.h
//  keyboard2
//
//  Created by Will Yager on 6/20/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#ifndef __keyboard2__Types__
#define __keyboard2__Types__

#include <inttypes.h>

//Maps a physical button number
//To a USB key spec number
struct key_mapping {
    //Physical button
    unsigned char button;
    //USB key data
    uint16_t key;
};

//Records when a key (the USB number, not the physical number)
//gets pressed or released
//Used by KeyboardComputerInterface
struct key_event{
    //Physical button state
    char state;
    //USB key data
    uint16_t key;
};

//Records when a button (the physical number)
//gets pressed or released
//Used by KeyboardHardwareInterface
struct button_event{
	uint8_t button_number;
	bool state;//true=pushed,false=released
};

//Holds a number of button_events
#define MAX_BUTTON_EVENTS_PER_SCAN 8
struct button_report{
	uint8_t num_events;
	//Up to 8 events per scan. This equates to about 4000 button press & releases per second.
	//The most I've *ever* been able to trigger at once is 5. We have plenty of wiggle room before this happens.
	button_event events[MAX_BUTTON_EVENTS_PER_SCAN];
};

#endif /* defined(__keyboard2__Types__) */
