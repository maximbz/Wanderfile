//
//  CSRandomRange.hpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/13/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSRandomRange_hpp
#define CSRandomRange_hpp

#include <stdio.h>
#include "WanderFile.h"
#include "CSRange.hpp"

class CSRandomRange
{
private:
    randType    _randType;
    CSRange     _range;
    
public:
    CSRandomRange();
    CSRandomRange(randType, int, int);
    CSRandomRange(randType, CSRange);
    
    void setRandType(randType);
    void setRange(CSRange);
    void setRangeMin(int);
    void setRangeMax(int);
    
    randType getRandType(void);
    CSRange getRange(void);
    int getRangeMin(void);
    int getRangeMax(void);
};

#endif /* CSRandomRange_hpp */
