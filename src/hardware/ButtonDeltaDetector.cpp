//  ButtonDeltaDetector.cpp
//  Created by Will Yager on 7/3/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//  Detects changes in button state

#include "hardware/ButtonDeltaDetector.h"

ButtonsDelta ButtonDeltaDetector::update(const ButtonsState& debounced_states){
    ButtonsDelta result;
    for (int i = 0; i < 64; ++i){
        result.deltas[i] = 0;
        bool known_state = this->known_states[i];
        bool debounced_state = debounced_states.states[i];
        if (known_state != debounced_state)
        {
        	known_states[i] = debounced_state;
        	result.deltas[i] = (debounced_state*2)-1;//1=>1, 0=>-1
        }
    }
    return result;
}