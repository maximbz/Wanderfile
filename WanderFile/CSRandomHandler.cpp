//
//  CSRandom.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/1/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSRandomHandler.hpp"
#include <stdio.h>

CSRandomHandler::CSRandomHandler(void) : _randomGenerator((unsigned int)time(0))
{
    
}


//doers

void CSRandomHandler::addRandomRange(CSRandomRange inRandRange)
{
    _ranges.push_back(inRandRange);
    //printf("%lu ", _ranges.size());
}

void CSRandomHandler::addRandomList(CSRandomList inRandList)
{
    _lists.push_back(inRandList);
}

void CSRandomHandler::clearRandomItems(randType inRandType)
{
    vector<CSRandomRange>::iterator rangeVectIter = _ranges.begin();
    vector<CSRandomList>::iterator  listVectIter = _lists.begin();
    
    while(rangeVectIter != _ranges.end())
        if((*rangeVectIter).getRandType() == inRandType)
            rangeVectIter = _ranges.erase(rangeVectIter);//new iterator properly goes through the list, now with fewer entries
    
    while(listVectIter != _lists.end())
        if((*listVectIter).getRandType() == inRandType)
            listVectIter = _lists.erase(listVectIter);//new iterator properly goes through the list, now with fewer entries
}


int CSRandomHandler::getNumber (CSRandomRange *inRange)
{
    int numberMin = 0, numberMax = 0;
    
    numberMin = inRange->getRangeMin();
    numberMax = inRange->getRangeMax();
    
    uniform_int_distribution<int>   distributionRange(numberMin, numberMax);
    
    return distributionRange(_randomGenerator);
}

int CSRandomHandler::getNumber (CSRandomList *inList)
{
    int numberMin = 0, numberMax = 0;
    
    numberMax = (int)inList->getList()->size() - 1;//we're using numberMax to determine the highest element in the vector, so we offset down one
    
    uniform_int_distribution<int>   distributionRange(numberMin, numberMax);
    
    return inList->getList()->at(distributionRange(_randomGenerator));
}
