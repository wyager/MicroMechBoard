//  ButtonDebouncer.cpp
//  Created by Will Yager on 7/3/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//  Responsible for debouncing hardware buttons

#include "ButtonDebouncer.h"

//Debounce raw hardware samples
ButtonsState ButtonDebouncer::update(const ButtonsState& raw_state){
    ButtonsState result;
    for (int i = 0; i < 64; ++i){
        bool debounced_state = this->debouncer.update(i, raw_state.states[i]);
        result.states[i] = debounced_state;
    }
    return result;
}