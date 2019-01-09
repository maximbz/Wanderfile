//
//  CSRandomList.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 10/6/18.
//  Copyright © 2018 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSRandomList.hpp"

CSRandomList::CSRandomList()
{
    
}

CSRandomList::CSRandomList(randType inRandType)
{
    _randType = inRandType;
}

CSRandomList::CSRandomList(randType inRandType, vector<int>* inList)
{
    _randType = inRandType;
    _numList = *inList;
}


void CSRandomList::setRandType(randType inRandType)
{
    _randType = inRandType;
}

void CSRandomList::setList(vector<int>* inList)
{
    _numList = *inList;
}


void CSRandomList::addNumToList(int inNum)
{
    _numList.push_back(inNum);
}

void CSRandomList::addListToList(vector<int>* inList)
{
    _numList.insert(_numList.begin(), inList->begin(), inList->end());
}

randType CSRandomList::getRandType(void)
{
    return _randType;
}

vector<int>* CSRandomList::getList(void)
{
    return &_numList;
}
