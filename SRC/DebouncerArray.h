//
//  DebouncerArray.h
//  keyboard
//
//  Created by Will Yager on 6/17/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#ifndef __keyboard__DebouncerArray__
#define __keyboard__DebouncerArray__

template <int Size>
class DebouncerArray {
protected:
    //The number of samples that conflict with the current recorded state
    unsigned char count[Size];
    //The current recorded state
    signed char state[Size];
    //How many contrary samples it takes to flip the state
    unsigned char sensitivity;
    
public:
    //Sensitivity is how long it takes to flip the state
    DebouncerArray(unsigned char sensitivity){
        for(int i=0; i<Size; i++){
            this->count[i]=0;
            this->state[i]=0;
        }
        this->sensitivity = sensitivity;
    }
    //Update with a state value. Should be either 0 or 1
    //Returns -1 on error, and element value otherwise
    virtual signed char update(int element, signed char state);
    //Gets the value of an element
    signed char get(int element);
};

template <int Size>
signed char DebouncerArray<Size>::update(int element, signed char state){
    if (element >= Size || element < 0){
        return -1;
    }
    //Sample reaffirms existing recorded state
    if(this->state[element] == state){
        if(this->count[element] > 0){
            this->count[element] -= 1;
        }
    }
    //Sample conflicts with existing recorded state
    if(this->state[element] != state){
        this->count[element] += 1;
        if(this->count[element] >= this->sensitivity){
            this->state[element] = !this->state[element];
            this->count[element] = 0;
        }
    }
    return this->state[element];
}

template <int Size>
signed char DebouncerArray<Size>::get(int element){
    return this->state[element];
}

#endif /* defined(__keyboard__DebouncerArray__) */
