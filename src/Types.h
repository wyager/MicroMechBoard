//  BoolArray.h
//  Created by Will Yager on 6/20/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.

#ifndef __keyboard2__Types__
#define __keyboard2__Types__

#include <inttypes.h>

//Holds information for up to 64 buttons
struct ButtonsState{
    bool states[64];
};
//Tells us whether a button has changed state
struct ButtonsDelta{
    //-1 for released, 1 for pressed, 0 for no change
    int8_t deltas[64];
};
//Tells us whether a key (from USB spec) has changed state
struct KeyDelta{
    //USB keycodes
    uint16_t key;
    //-1 for released, 1 for pressed, 0 for no change
    int8_t delta;
};
//Tells us whether a bunch of keys (from USB spec) has changed state
struct KeysDelta{
    KeyDelta deltas[64];
};

#endif /* defined(__keyboard2__Types__) */
