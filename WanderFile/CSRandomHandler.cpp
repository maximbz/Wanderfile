//
//  CSRandom.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/1/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSRandomHandler.hpp"
#include <stdio.h>

CSRandomHandler::CSRandomHandler(void) : _randomGenerator((unsigned int)time(0)){}


//doers

void CSRandomHandler::addRandomRange(CSRandomRange inRandRange)
{
    int loop;
    
    //check for dupes from previous dungeon
    for(loop = 0; loop < _ranges.size(); loop++)
        if(inRandRange.getRangeName() == _ranges[loop].getRangeName())
            _ranges.erase(_ranges.begin() + loop);
    
    _ranges.push_back(inRandRange);
    //printf("%lu ", _ranges.size());
}

int CSRandomHandler::getNumber(string inRange)
{
    int numberMin = 0, numberMax = 0;
    
    vector<CSRandomRange>::iterator vectIter;
    
    for(vectIter = _ranges.begin(); vectIter != _ranges.end(); vectIter++)
        if(inRange == vectIter->getRangeName())
        {
            numberMin = vectIter->getRangeMin();
            numberMax = vectIter->getRangeMax();
        }
    
    uniform_int_distribution<int>   distributionRange(numberMin, numberMax);
    
    return distributionRange(_randomGenerator);
}

int CSRandomHandler::getNumber (CSRandomRange *inRange)
{
    int numberMin = 0, numberMax = 0;
    
    numberMin = inRange->getRangeMin();
    numberMax = inRange->getRangeMax();
    
    uniform_int_distribution<int>   distributionRange(numberMin, numberMax);
    
    return distributionRange(_randomGenerator);
}
