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

CSRandomRange::CSRandomRange(string incomingName, int incomingRangeMin, int incomingRangeMax)
{
    _rangeName = incomingName;
    
    _range.setRange(incomingRangeMin, incomingRangeMax);
}

CSRandomRange::CSRandomRange(string incomingName, CSRange incomingRange)
{
    _rangeName = incomingName;
    _range = incomingRange;
}


void CSRandomRange::setName(string incomingName)
{
    _rangeName = incomingName;
}

void CSRandomRange::setRange(CSRange incomingRange)
{
    _range = incomingRange;
}

void CSRandomRange::setRangeMin(int incomingMin)
{
    _range.setMin(incomingMin);
}

void CSRandomRange::setRangeMax(int incomingMax)
{
    _range.setMin(incomingMax);
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
