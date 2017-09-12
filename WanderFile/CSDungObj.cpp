//
//  CSDungObj.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 11/20/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSDungObj.hpp"
#include "CSRoom.hpp"

CSDungObj::CSDungObj()
{
    
}

CSDungObj::CSDungObj(char incomingChar, int incomingType, objReg incomingRegion, CSPoint incomingLoc, CSRoom *incomingOwner)//new objects (not doors)
{
    _objectChar = incomingChar;
    _objectType = incomingType;
    _objectRegion = incomingRegion;
    _objectLoc = incomingLoc;
    _connectTo = nullptr;
    _owner = incomingOwner;
}

CSDungObj::CSDungObj(objReg incomingDoorWall, CSPoint incomingLoc, CSDungObj *incConnect, CSRoom *incomingOwner)//new doors
{
    _objectChar = OPEN_DOOR_CHAR;
    _objectType = OBJ_TYPE_DOOR;
    _objectRegion = incomingDoorWall;
    _objectLoc = incomingLoc;
    setConnectTo(incConnect);
    _owner = incomingOwner;
}


//setters

void CSDungObj::setChar(char incomingChar)
{
    _objectChar = incomingChar;
}

void CSDungObj::setType(int incomingType)
{
    _objectType = incomingType;
}

void CSDungObj::setRegion(objReg incomingRegion)
{
    _objectRegion = incomingRegion;
}

void CSDungObj::setLoc(CSPoint incomingLoc)
{
    _objectLoc = incomingLoc;
}

void CSDungObj::setConnectTo(CSDungObj *incConnect)
{
    _connectTo = incConnect;
    
    if(incConnect != nullptr)
        incConnect->setConnectFrom(this);
}

void CSDungObj::setConnectFrom(CSDungObj *incConnect)
{
    _connectFrom = incConnect;
}

void CSDungObj::setOwner(CSRoom *incomingOwner)
{
    _owner = incomingOwner;
}


//doers

bool CSDungObj::slideObject(CSPoint incomingVector)
{
    bool                goodLoc = true, axis = getWallAxis(_objectRegion);
    CSPoint             newLoc = (_objectLoc + incomingVector);
    CSRect              newRoomRect;
    list<CSDungObj*>    *wallDoors;
    
    list<CSDungObj*>::iterator  listIter;
    
    //if the door is attempting to be slid along the perp axis, away from the wall...
    if(incomingVector.getAxisPoint(!axis) != 0)
    {
        newRoomRect = *_owner->getRect();
        newRoomRect.setWallLoc(_objectRegion, newLoc.getAxisPoint(!axis));//move the wall this door is on to the same spot
        if(newRoomRect.getDim(!axis) >= HALL_SIZE)//don't let the room be squished to nothing
            _owner->getRect()->setPoints(newRoomRect.topLeft, newRoomRect.botRight);
        else
            return false;
        
        //loop through all of the moved wall's connected doors, see if they can be slid
        wallDoors = _owner->getObjects();
        for(listIter = wallDoors->begin(); listIter != wallDoors->end(); listIter++)
        {
            //if it's a non-null door on the wall that was moved
            if((*listIter)->getRegion() == _objectRegion && (*listIter)->getConnect() != nullptr)
                if(!(*listIter)->getConnect()->slideObject(incomingVector))//we try to slide its connecting door. If we can't...
                {
                    goodLoc = false;
                    break;
                }
        }
        
    }//if we come out the other side of this if, then this slide worked so far
    
    //if the door is attempting to be slid along the para axis, within the wall...
    if(incomingVector.getAxisPoint(getWallAxis(_objectRegion)) != 0)
        goodLoc = _owner->isPointInFreeWall(newLoc, _objectRegion);//check that newDoorLoc is in free-wall
    
    //if the intended location is within any free-wall range, we set ourselves to it and return that we were successful
    if(goodLoc)
        _objectLoc = newLoc;
    
    return goodLoc;
}

void CSDungObj::deleteObject(void)
{
    if(_connectTo != nullptr)
    {
        _connectTo->setConnectFrom(nullptr);
        if(_objectType == OBJ_TYPE_DOOR)
            _connectTo->setConnectTo(nullptr);
    }
    
    delete this;
}


//getters

char CSDungObj::getChar(void)
{
    return _objectChar;
}

int CSDungObj::getType(void)
{
    return _objectType;
}

objReg CSDungObj::getRegion(void)
{
    return _objectRegion;
}

CSPoint* CSDungObj::getLoc(void)
{
    return &_objectLoc;
}

CSDungObj* CSDungObj::getConnect(void)
{
    return _connectTo;
}

CSRoom* CSDungObj::getOwner(void)
{
    return _owner;
}
