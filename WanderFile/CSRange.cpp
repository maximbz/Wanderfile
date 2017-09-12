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
        min = incomingMin;
        max = incomingMax;
    }
    else
    {
        max = incomingMin;
        min = incomingMax;
    }
}


bool CSRange::doesContain(int incomingPoint)
{
    return incomingPoint >= min && incomingPoint <= max;
}


int CSRange::getSize()
{
    return max - min;
}
