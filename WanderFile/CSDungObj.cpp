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
    _parentObj = nullptr;
    _childObj = nullptr;
    _connect = nullptr;
    _owner = nullptr;
}

CSDungObj::CSDungObj(char incomingChar, objType incomingType, objReg incomingRegion, CSPoint incomingLoc, CSDungObj *incParent, CSDungObj *incConnect, CSRoom *incomingOwner)
{
    _objectChar = incomingChar;
    _objectType = incomingType;
    _objectRegion = incomingRegion;
    _objectLoc = incomingLoc;
    
    setParent(incParent);
    _childObj = nullptr;
    setConnect(incConnect);
    _owner = incomingOwner;
    
    _wasMoved = false;
}


#pragma mark -
#pragma mark Setters

void CSDungObj::setWasMoved(bool incomingState)
{
    _wasMoved = incomingState;
}

void CSDungObj::setChar(char incomingChar)
{
    _objectChar = incomingChar;
}

void CSDungObj::setType(objType incomingType)
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

void CSDungObj::setParent(CSDungObj *incomingParent)
{
    _parentObj = incomingParent;
    
    if(incomingParent != nullptr)
        if(_parentObj->getChild() != this)
            _parentObj->setChild(this);
}

void CSDungObj::setChild(CSDungObj *incomingChild)
{
    _childObj = incomingChild;
    
    if(incomingChild != nullptr)
        if(_childObj->getParent() != this)
            _childObj->setParent(this);
    
    if(_objectType == OBJ_DOOR)
        incomingChild = nullptr;
}

void CSDungObj::setConnect(CSDungObj *incConnect)
{
    _connect = incConnect;
    
    if(incConnect != nullptr)
        if(_connect->getConnect() != this)
            incConnect->setConnect(this);
}

void CSDungObj::setOwner(CSRoom *incomingOwner)
{
    _owner = incomingOwner;
}


#pragma mark -
#pragma mark Doers

void CSDungObj::slideObject(CSPoint incomingVector)
{
    CSPoint newLoc = _objectLoc + incomingVector;
    bool    slideUp = false;
    
    //if we have a parent, pass this up the chain to them. If we don't set our loc, then go down the chain, and move our child on the same vector we were moved
    if(_parentObj != nullptr)
    {
        if(!_parentObj->getWasMoved())
            slideUp = true;
    }
    
    if(slideUp)
        _parentObj->slideObject(incomingVector);
    else
    {
        _wasMoved = true;
        setLoc(newLoc);
        
        if(_childObj != nullptr)
            _childObj->slideObject(incomingVector);
    }
}

bool CSDungObj::slideDoor(CSPoint incomingVector)
{
    bool    goodLoc = true;
    axis    axis = getWallAxis(_objectRegion);
    CSPoint newLoc = (_objectLoc + incomingVector);
    CSRange wallMovementRange;
    CSRect  newRoomRect;
    CSDungObj   *objectToCheck;
    
    list<CSDungObj*>            *roomObjects;
    list<CSDungObj*>::iterator  listIter;
    
    _wasMoved = true;
    
    //if the door is attempting to be slid along the perp axis, away from the wall...
    if(incomingVector.getAxisPoint(getPerpAxis(axis)) != 0)
    {
        //move the wall this door is on to the same spot
        newRoomRect = *_owner->getRect();
        newRoomRect.setWallLoc(_objectRegion, newLoc.getAxisPoint(getPerpAxis(axis)));
        
        //Before the wall can be slid, check if there are any doors on the tiles between the wall's current and future loc.
        wallMovementRange.setRange(newRoomRect.getWallLocPoint(_objectRegion), _owner->getRect()->getWallLocPoint(_objectRegion));
        roomObjects = _owner->getObjects();
        for(listIter = roomObjects->begin(); listIter != roomObjects->end(); listIter++)
        {
            if((*listIter)->getType() != OBJ_DOOR || (*listIter)->getRegion() == _objectRegion)
                continue;
            
            //If there are, cancel the move
            objectToCheck = (*listIter);
            if(wallMovementRange.doesContain((*listIter)->getLoc()->getAxisPoint(getPerpAxis(axis))))
                return false;
        }
        
        //don't let the room be squished to nothing
        if(newRoomRect.getDim(getPerpAxis(axis)) >= HALL_SIZE)
            _owner->getRect()->setPoints(newRoomRect.topLeft, newRoomRect.botRight);
        else
            return false;
        
        //loop through all of the moved wall's doors, see if they can be slid
        for(listIter = roomObjects->begin(); listIter != roomObjects->end(); listIter++)
            if((*listIter)->checkForRegion(_objectRegion) && !(*listIter)->getWasMoved())//if it's an object that should be moved, but hasn't yet been
            {
                if((*listIter)->getType() == OBJ_DOOR)
                {
                    goodLoc = (*listIter)->slideDoor(incomingVector);//we try to slide it. If we can't...
                    if(!goodLoc)
                        break;
                }
                else//if it's not a door (like it's anchored to the wall that's being moved, e.g.: a room num)
                    (*listIter)->slideObject(incomingVector);
            }
        
        //reset all objects' _wasMoved to false;
        for(listIter = roomObjects->begin(); listIter != roomObjects->end(); listIter++)
            (*listIter)->setWasMoved(false);
    }//if we come out the other side of this if, then this slide worked so far
    
    //if the door is attempting to be slid along the para axis, within the wall...
    if(incomingVector.getAxisPoint(getWallAxis(_objectRegion)) != 0)
        goodLoc = _owner->isPointInFreeWall(newLoc, _objectRegion);//check that newDoorLoc is in free-wall
    
    //if the intended location is within any free-wall range, we set ourselves to it and return that we were successful
    if(goodLoc)
        slideObject(incomingVector);
    
    return goodLoc;
}

bool CSDungObj::checkForRegion(objReg incomingReg)
{
    if(_objectRegion == REG_CORNER_TOP_LEFT)
        switch(incomingReg)
        {
            case REG_CORNER_TOP_LEFT:
            case REG_WALL_TOP:
            case REG_WALL_LEFT:
                return true;
                break;
            default:
                return false;
        }
    else
        if(_objectRegion == incomingReg)
            return true;
        else
            return false;
}

void CSDungObj::deleteObject(void)
{
    if(_parentObj!= nullptr)
        _parentObj->setChild(nullptr);
    
    if(_childObj!= nullptr)
        _childObj->setParent(nullptr);
    
    if(_connect != nullptr)
        _connect->setConnect(nullptr);
    
    delete this;
}


#pragma mark -
#pragma mark Getters

bool CSDungObj::getWasMoved(void)
{
    return _wasMoved;
}

char CSDungObj::getChar(void)
{
    return _objectChar;
}

objType CSDungObj::getType(void)
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

CSDungObj* CSDungObj::getParent(void)
{
    return _parentObj;
}

CSDungObj* CSDungObj::getChild(void)
{
    return _childObj;
}

CSDungObj* CSDungObj::getConnect(void)
{
    return _connect;
}

CSRoom* CSDungObj::getOwner(void)
{
    return _owner;
}








