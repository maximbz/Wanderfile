//
//  CSRandomRange.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/13/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSRandomRange.hpp"

CSRandomRange::CSRandomRange()
{
    
}

CSRandomRange::CSRandomRange(randType inRandType, int inRangeMin, int inRangeMax)
{
    _randType = inRandType;
    _range.setRange(inRangeMin, inRangeMax);
}

CSRandomRange::CSRandomRange(randType inRandType, CSRange inRange)
{
    _randType = inRandType;
    _range = inRange;
}


void CSRandomRange::setRandType(randType inRandType)
{
    _randType = inRandType;
}

void CSRandomRange::setRange(CSRange inRange)
{
    _range = inRange;
}

void CSRandomRange::setRangeMin(int inMin)
{
    _range.setMin(inMin);
}

void CSRandomRange::setRangeMax(int inMax)
{
    _range.setMin(inMax);
}


randType CSRandomRange::getRandType(void)
{
    return _randType;
}

CSRange CSRandomRange::getRange(void)
{
    return _range;
}

int CSRandomRange::getRangeMin(void)
{
    return _range.getMin();
}

int CSRandomRange::getRangeMax(void)
{
    return _range.getMax();
}
