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

void CSRandomHandler::addRandomRange(CSRandomRange incomingRandomRange)
{
    int loopCounter;
    
    //check for dupes from previous dungeon
    for(loopCounter = 0; loopCounter < _ranges.size(); loopCounter++)
        if(incomingRandomRange.getRangeName() == _ranges[loopCounter].getRangeName())
            _ranges.erase(_ranges.begin() + loopCounter);
    
    _ranges.push_back(incomingRandomRange);
    //printf("%lu ", _ranges.size());
}

int CSRandomHandler::getNumber(string incomgingRange)
{
    int numberMin = 0, numberMax = 0;
    
    vector<CSRandomRange>::iterator vectIter;
    
    for(vectIter = _ranges.begin(); vectIter != _ranges.end(); vectIter++)
        if(incomgingRange == vectIter->getRangeName())
        {
            numberMin = vectIter->getRangeMin();
            numberMax = vectIter->getRangeMax();
        }
    
    uniform_int_distribution<int>   distributionRange(numberMin, numberMax);
    
    return distributionRange(_randomGenerator);
}

int CSRandomHandler::getNumber (CSRandomRange *incomingRange)
{
    int numberMin = 0, numberMax = 0;
    
    numberMin = incomingRange->getRangeMin();
    numberMax = incomingRange->getRangeMax();
    
    uniform_int_distribution<int>   distributionRange(numberMin, numberMax);
    
    return distributionRange(_randomGenerator);
}
