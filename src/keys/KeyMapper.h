//  KeyMapper.h
//  Created by Will Yager on 7/3/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//  Maps physical button numbers to USB key codes

#ifndef __keyboard2__KeyMapper__
#define __keyboard2__KeyMapper__

#include "Types.h"


class KeyMapper {
private:
	uint16_t map_button_to_key(uint8_t button);
public:
    //Takes in a ButtonsDelta, finds any changes, and spits out a KeysDelta
    //With those changes
	KeysDelta resolve(const ButtonsDelta& button_changes);
};

#endif /* defined(__keyboard2__KeyMapper__) */
