//  ButtonDebouncer.h
//  Created by Will Yager on 7/3/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//  Responsible for debouncing hardware buttons

#ifndef __keyboard2__ButtonDebouncer__
#define __keyboard2__ButtonDebouncer__

#include "Types.h"
#include "DebouncerArray.h"
class ButtonDebouncer {
private:
	DebouncerArray<64> debouncer;
public:
	//The threshold is the number of samples it takes to
	//convince the debouncer that something is true
	ButtonDebouncer(uint8_t threshold):debouncer(threshold){}
	//Debounce raw hardware samples
	ButtonsState update(const ButtonsState& raw_state);
};

#endif /* defined(__keyboard2__ButtonDebouncer__) */
