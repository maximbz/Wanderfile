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

CSRandomRange::CSRandomRange(string inName, int inRangeMin, int inRangeMax)
{
    _rangeName = inName;
    
    _range.setRange(inRangeMin, inRangeMax);
}

CSRandomRange::CSRandomRange(string inName, CSRange inRange)
{
    _rangeName = inName;
    _range = inRange;
}


void CSRandomRange::setName(string inName)
{
    _rangeName = inName;
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
    return _range.getMin();
}

int CSRandomRange::getRangeMax(void)
{
    return _range.getMax();
}
