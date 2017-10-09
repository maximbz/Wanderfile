//
//  CSRange.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 6/15/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSRange.hpp"

CSRange::CSRange()
{
    _min = 0;
    _max = 0;
}

CSRange::CSRange(int incomingMin, int incomingMax)
{
    setRange(incomingMin, incomingMax);
}


void CSRange::setRange(int incomingMin, int incomingMax)
{
    //never allow the min to be greater than the max
    if(incomingMin < incomingMax)
    {
        _min = incomingMin;
        _max = incomingMax;
    }
    else
    {
        _max = incomingMin;
        _min = incomingMax;
    }
}

void CSRange::setMin(int incomingMin)
{
    //never allow the min to be greater than the max
    if(incomingMin <= _max)
        _min = incomingMin;
    else
    {
        _min = _max;
        _max = incomingMin;
    }
}

void CSRange::setMax(int incomingMax)
{
    //never allow the min to be greater than the max
    if(incomingMax >= _min)
        _max = incomingMax;
    else
    {
        _max = _min;
        _min = incomingMax;
    }
}


bool CSRange::doesContain(int incomingPoint)
{
    return incomingPoint >= _min && incomingPoint <= _max;
}


int CSRange::getMin(void)
{
    return _min;
}

int CSRange::getMax(void)
{
    return _max;
}

int CSRange::getSize(void)
{
    return _max - _min;
}
