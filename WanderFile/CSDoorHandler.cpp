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
    int         loop;
    vector<int> newDoorOddsList;
    
    _theRandHand = inRandHand;
    
    for(loop = 0; loop < NUM_ONE_DOOR_CHANCES; loop++)
        newDoorOddsList.push_back(1);
    for(loop = 0; loop < NUM_TWO_DOOR_CHANCES; loop++)
        newDoorOddsList.push_back(2);
    for(loop = 0; loop < NUM_THREE_DOOR_CHANCES; loop++)
        newDoorOddsList.push_back(3);
    _numNewDoors.addListToList(&newDoorOddsList);
    _numNewDoors.setRandType(RAND_DUNGEON);
    _theRandHand->addRandomList(_numNewDoors);
    
    _numDoors = 0;
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
    
    if(_unconnectedLevelDoors.size() == 0)
        return nullptr;
    
    doorAtBack = _unconnectedLevelDoors.back();
    
    _numDoors--;
    _unconnectedLevelDoors.pop_back();
    
    return doorAtBack;
}

void CSDoorHandler::removeDoor(CSDungObj *inDoor)
{
    list<CSDungObj*>::iterator doorIter;
    
    for(doorIter = _unconnectedLevelDoors.begin(); doorIter != _unconnectedLevelDoors.end(); doorIter++)
        if((*doorIter) == inDoor)
        {
            //for debug
            //CSDungObj   *doorToDelete = *doorIter;
            //printf("Removing object num %d from room num %d.\n", doorToDelete->getNum(), doorToDelete->getOwner()->getRoomNum());
            
            _unconnectedLevelDoors.erase(doorIter);
            _numDoors--;
            return;
        }
}

int CSDoorHandler::getNewDoorQuantity(void)
{
    return _theRandHand->getNumber(&_numNewDoors);
}


int CSDoorHandler::getNumDoors(void)
{
    return _numDoors;
}

int CSDoorHandler::getSize(void)
{
    return (int)_unconnectedLevelDoors.size();
}
