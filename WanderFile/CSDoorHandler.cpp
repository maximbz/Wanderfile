//
//  CSDoorHandler.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 11/18/18.
//  Copyright © 2018 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include <vector>
#include "CSDoorHandler.hpp"
#include "CSRoom.hpp"

CSDoorHandler::CSDoorHandler(CSRandomHandler *inRandHand)
{
    int         loop, subLoop, subLoopTotal;
    vector<int> newDoorOddsList;
    
    _theRandHand = inRandHand;
    _numDoors = 0;
    
    //make and add the normal door qty gen list
    for(loop = 1; loop < NUM_ROOM_WALLS; loop++)
    {
        subLoopTotal = ONE_DOOR_CHANCES / (pow(loop, 2));//30, 7, 3
        for(subLoop = 0; subLoop < subLoopTotal; subLoop++)
            newDoorOddsList.push_back(loop);
    }
    _numNewDoorsRandList.addListToList(&newDoorOddsList);
    _numNewDoorsRandList.setRandType(RAND_DUNGEON);
    _theRandHand->addRandomList(_numNewDoorsRandList);
    
    //make and add the "too many doors" door qty gen range (50/50, 0 or 1)
    _newDoorRandRange.setRangeMin(0);
    _newDoorRandRange.setRangeMax(1);
    _newDoorRandRange.setRandType(RAND_DUNGEON);
    _theRandHand->addRandomRange(_newDoorRandRange);
}


void CSDoorHandler::clear(void)
{
    //erases and deletes all objects, removes the reference back to this CSRoom from all connected CSRoom's
    while(!_unconnectedLevelDoors.empty())
        _unconnectedLevelDoors.pop_back();
}


void CSDoorHandler::addDoor(CSDungObj *inDoor)
{
    if(inDoor->getType() != OBJ_DOOR)
        return;
    
    _numDoors++;
    _unconnectedLevelDoors.push_back(inDoor);
}

CSDungObj* CSDoorHandler::getNextDoor(void)
{
    CSDungObj   *doorAtBack;
    
    if(_unconnectedLevelDoors.size() == 0)//we are out of doors!
        return nullptr;
    
    //pull the next door from the list
    doorAtBack = _unconnectedLevelDoors.back();
    _unconnectedLevelDoors.pop_back();
    _numDoors--;
    
    return doorAtBack;
}

void CSDoorHandler::removeDoor(CSDungObj *inDoor)
{
    list<CSDungObj*>::iterator doorIter;
    
    //used when aborting room gen paths, we remove the incoming door from the list
    for(doorIter = _unconnectedLevelDoors.begin(); doorIter != _unconnectedLevelDoors.end(); doorIter++)
        if((*doorIter) == inDoor)
        {
            _unconnectedLevelDoors.erase(doorIter);
            _numDoors--;
            return;
        }
}

int CSDoorHandler::getNewDoorQuantity(int inDoorsRemaining)
{
    int numNewDoors, loop;
    
    if(_numDoors < inDoorsRemaining * DOOR_GEN_SLOW_POINT)//normal mode
    {
        numNewDoors = _theRandHand->getNumber(&_numNewDoorsRandList);//1 door is likely, 2 is occasional, 3 is rare
        
        //the number of doors NOT choosen become more likely
        for(loop = 1; loop < NUM_ROOM_WALLS; loop++)
            if(numNewDoors != loop)
                _numNewDoorsRandList.addNumToList(loop);
    }
    else//if we're close to hitting max num doors, we pull back
        numNewDoors = _theRandHand->getNumber(&_newDoorRandRange);//1 or 0--either maintain this room path or make a dead-end
    
    return numNewDoors;
}


int CSDoorHandler::getNumDoors(void)
{
    return _numDoors;
}

int CSDoorHandler::getSize(void)
{
    return (int)_unconnectedLevelDoors.size();
}
