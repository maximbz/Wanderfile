//
//  CSDungObj.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 11/20/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSDungObj.hpp"
#include "CSRoom.hpp"
#include "CSAxis.hpp"

CSDungObj::CSDungObj()
{
    _parentObj = nullptr;
    _childObj = nullptr;
    _connect = nullptr;
    _owner = nullptr;
}

CSDungObj::CSDungObj(objType inType, objReg inRegion, CSPoint *inLoc, CSDungObj *inParent, CSDungObj *inConnect, CSRoom *inOwner)
{
    _objectType = inType;
    _objectRegion = inRegion;
    if(inLoc != nullptr)
        _objectLoc = *inLoc;
    
    setParent(inParent);
    _childObj = nullptr;
    setConnect(inConnect);
    _owner = inOwner;
    
    _wasMoved = false;
}


#pragma mark -
#pragma mark Setters

void CSDungObj::setWasMoved(bool inState)
{
    _wasMoved = inState;
}

void CSDungObj::setChar(char inChar)
{
    _objChar = inChar;
}

void CSDungObj::setNum(int inObjNum)
{
    _objNum = inObjNum;
}

void CSDungObj::setLoc(CSPoint inLoc)
{
    _objectLoc = inLoc;
}

void CSDungObj::setParent(CSDungObj *inParent)
{
    _parentObj = inParent;
    
    if(inParent != nullptr)
        if(_parentObj->getChild() != this)
            _parentObj->setChild(this);
}

void CSDungObj::setChild(CSDungObj *inChild)
{
    _childObj = inChild;
    
    if(inChild != nullptr)
        if(_childObj->getParent() != this)
            _childObj->setParent(this);
    
    if(_objectType == OBJ_DOOR)
        inChild = nullptr;
}

void CSDungObj::setConnect(CSDungObj *inConnect)
{
    _connect = inConnect;
    
    if(inConnect != nullptr)
        if(_connect->getConnect() != this)
            inConnect->setConnect(this);
}


#pragma mark -
#pragma mark Doers

void CSDungObj::slideObject(CSPoint inVector)
{
    CSPoint newLoc = _objectLoc + inVector;
    bool    slideUp = false;
    
    //if we have a parent, pass this up the chain to them. If we don't set our loc, then go down the chain, and move our child on the same vector we were moved
    if(_parentObj != nullptr)
    {
        if(!_parentObj->getWasMoved())
            slideUp = true;
    }
    
    if(slideUp)
        _parentObj->slideObject(inVector);
    else
    {
        _wasMoved = true;
        setLoc(newLoc);
        
        if(_childObj != nullptr)
            _childObj->slideObject(inVector);
    }
}

bool CSDungObj::slideDoor(CSPoint inVector)
{
    bool    goodLoc = true;
    CSAxis  wallAxis;
    CSPoint newLoc = (_objectLoc + inVector);
    
    list<CSDungObj*>            *roomObjects;
    list<CSDungObj*>::iterator  listIter;
    
    wallAxis.setAxisFromWall(_objectRegion);
    _wasMoved = true;
    
    //if the door is attempting to be slid along the perp axis, away from the wall...
    if(inVector.getAxisPoint(wallAxis.getPerpAxis()) != 0)
    {
        if(!_owner->slideWall(_objectRegion, inVector.getAxisPoint(wallAxis.getPerpAxis())))
            return false;
        
        //loop through all of the moved wall's doors, see if they can be slid
        roomObjects = _owner->getObjects();
        for(listIter = roomObjects->begin(); listIter != roomObjects->end(); listIter++)
            if((*listIter)->checkForRegion(_objectRegion) && !(*listIter)->getWasMoved())//if it's an object that should be moved, but hasn't yet been
            {
                if((*listIter)->getType() == OBJ_DOOR)
                {
                    goodLoc = (*listIter)->slideDoor(inVector);//we try to slide it. If we can't...
                    if(!goodLoc)
                        break;
                }
                else//if it's not a door (like it's anchored to the wall that's being moved, e.g.: a room num)
                    (*listIter)->slideObject(inVector);
            }
        
        //reset all objects' _wasMoved to false;
        for(listIter = roomObjects->begin(); listIter != roomObjects->end(); listIter++)
            (*listIter)->setWasMoved(false);
    }//if we come out the other side of this if, then this slide worked so far
    
    //if the door is attempting to be slid along the para axis, within the wall...
    if(inVector.getAxisPoint(wallAxis.dim) != 0)
        goodLoc = _owner->isWallPointFree(&newLoc, _objectRegion, this);//check that newDoorLoc is in free-wall. We're okay with with hallway walls, since we'll necessarily be checking a spot for the door that's in a wall.
    
    //if the intended location is within any free-wall range, we set ourselves to it and return that we were successful
    if(goodLoc)
        slideObject(inVector);
    
    return goodLoc;
}

bool CSDungObj::checkForRegion(objReg inReg)
{
    if(_objectRegion == REG_CORNER_TOP_LEFT)
        switch(inReg)
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
        if(_objectRegion == inReg)
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

int CSDungObj::getNum(void)
{
    return _objNum;
}

char CSDungObj::getChar(void)
{
    //set the char based on type
    switch(_objectType)
    {
        case OBJ_ROOM_NUM:
            return _objChar;
        case OBJ_CREATURE:
            return _objChar;
        case OBJ_DOOR:
            return OPEN_DOOR_CHAR;
        case OBJ_STAIRS_UP:
            return STAIRS_UP_CHAR;
        case OBJ_STAIRS_DOWN:
            return STAIRS_DOWN_CHAR;
        case OBJ_TREASURE:
            return CHEST_CHAR;
            
        default:
            return EMPTY_CHAR;
    }
}

objType CSDungObj::getType(void)
{
    return _objectType;
}

bool CSDungObj::isPassable(void)
{
    switch(_objectType)
    {
        case OBJ_ROOM_NUM:
            return true;
        case OBJ_CREATURE:
            return false;
        case OBJ_DOOR:
            return true;
        case OBJ_STAIRS_UP:
            return true;
        case OBJ_STAIRS_DOWN:
            return true;
        case OBJ_TREASURE:
            return false;
        
        default:
            return true;
    }
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








