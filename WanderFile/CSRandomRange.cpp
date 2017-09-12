//
//  CSRandomRange.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/13/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSRandomRange.hpp"

CSRandomRange::CSRandomRange(string incomingName, int incomingRangeMin, int incomingRangeMax)
{
    _rangeName = incomingName;
    
    if(incomingRangeMax < incomingRangeMin)
        printf("RandomRange created with max below min. This will create unexpected results.\n");
    
    _range.min = incomingRangeMin;
    _range.max = incomingRangeMax;
}

CSRandomRange::CSRandomRange(string incomingName, CSRange incomingRange)
{
    _rangeName = incomingName;
    
    if(incomingRange.max < incomingRange.min)
        printf("RandomRange created with max below min. This will create unexpected results.\n");
    
    _range = incomingRange;
}


string CSRandomRange::getRangeName(void)
{
    return _rangeName;
}

CSRange CSRandomRange::getRange(void)
{
    return _range;
}

int CSRandomRange::getRangeMin(void)
{
    return _range.min;
}

int CSRandomRange::getRangeMax(void)
{
    return _range.max;
}
