//
//  BoolArray.h
//  keyboard2
//
//  Created by Will Yager on 6/20/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#ifndef __keyboard2__BoolArray__
#define __keyboard2__BoolArray__

#include <inttypes.h>
template <int Size>
class BoolArray {
public:
    uint8_t chunks[(Size+7)/8];
    const int numchunks;
    //returns 0 or 1 or -1 (on error)
    char get(int index);
    //if value != 0, it's 1
    void set(int index, char value);
    
    BoolArray<Size>():numchunks((Size+7)/8){
        for(int i=0; i<numchunks; i++){
            chunks[i]=0;
        }
    }
};

template <int Size>
char BoolArray<Size>::get(int index) {
    if(index > Size){
        return -1;
    }
    int chunk_index = index/8;
    int sub_index = index%8;
    char value = this->chunks[chunk_index] & (1 << (7-sub_index));
    value = (value != 0);
    return value;
}
//if value != 0, it's 1
template <int Size>
void BoolArray<Size>::set(int index, char value){
    if(index > Size){
        return;
    }
    int chunk_index = index/8;
    int sub_index = index%8;
    uint8_t old_chunk = this->chunks[chunk_index];
    if(value == 0){
        this->chunks[chunk_index] = old_chunk & ~(1 << (7-sub_index));
    }
    
    if(value != 0){
        this->chunks[chunk_index] = old_chunk | (1 << (7-sub_index));
    }
}

#endif /* defined(__keyboard2__BoolArray__) */
