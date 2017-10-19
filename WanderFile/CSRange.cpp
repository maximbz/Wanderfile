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

CSRange::CSRange(int inMin, int inMax)
{
    setRange(inMin, inMax);
}


void CSRange::setRange(int inMin, int inMax)
{
    //never allow the min to be greater than the max
    if(inMin < inMax)
    {
        _min = inMin;
        _max = inMax;
    }
    else
    {
        _max = inMin;
        _min = inMax;
    }
}

void CSRange::setMin(int inMin)
{
    //never allow the min to be greater than the max
    if(inMin <= _max)
        _min = inMin;
    else
    {
        _min = _max;
        _max = inMin;
    }
}

void CSRange::setMax(int inMax)
{
    //never allow the min to be greater than the max
    if(inMax >= _min)
        _max = inMax;
    else
    {
        _max = _min;
        _min = inMax;
    }
}


bool CSRange::doesContain(int inPoint)
{
    return inPoint >= _min && inPoint <= _max;
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
