//
//  CSEntity.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 11/20/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include <sstream>
#include "CSEntity.hpp"
#include "CSGameState.hpp"
#include "CSRoom.hpp"
#include "CSLine.hpp"
#include "CSAxis.hpp"

CSEntity::CSEntity(void)
{
    _parentEnt = nullptr;
    _childEnt = nullptr;
    _connect = nullptr;
    _owner = nullptr;
    _wasMoved = false;
    
    dataKeysInit();
}

CSEntity::CSEntity(CSGameState *inGame, entType inType, entReg inRegion, CSPoint *inLoc, CSEntity *inParent, CSEntity *inConnect, CSRoom *inOwner)
{
    _theGame = inGame;
    _entityType = inType;
    _entityRegion = inRegion;
    if(inLoc != nullptr)
        _entLoc = *inLoc;
    _owner = inOwner;
    if(_owner != nullptr)
        _owner->addEntity(this);
    setParent(inParent);
    _childEnt = nullptr;
    setConnect(inConnect);
    _wasMoved = false;
    
    dataKeysInit();
}

CSEntity::CSEntity(CSGameState *inGame, CSRoom *inOwner, CSFileLoader *inFileLoader)//load entity from file
{
    bool        connectionToMake = false;
    int         roomNumTest = BAD_DATA, entNum = BAD_DATA, entTypeAsInt = BAD_DATA, regAsInt;
    CSPoint     entLoc;
    CSLine      connectData;//we'll use a CSLine to hold the connect code, the roomNum, and the entNum
    
    list<string>::iterator    listIter;
    
    dataKeysInit();
    
    _theGame = inGame;
    _owner = inOwner;
    _parentEnt = nullptr;
    _childEnt = nullptr;
    _connect = nullptr;
    _wasMoved = false;
    _entChar = 'c';//just give it something to overwrite the nonsense char it may start with
    
    //set all the entity data
    inFileLoader->getIntValueFromKey(_entDataKey[0], _entNum);
    inFileLoader->getIntValueFromKey(_entDataKey[1], entTypeAsInt);
    _entityType = (entType)entTypeAsInt;
    
    if(_entityType == ENT_CREATURE)//we need to stop making a non-creature entity
    {
        _owner = nullptr;//something easy to change as a flag for the CSRoom that made us to see it needs to make a CSCreature instead
        return;//stop any further entity creation
    }
    
    inFileLoader->getIntValueFromKey(_entDataKey[2], regAsInt);
    _entityRegion = (entReg)regAsInt;
    inFileLoader->getIntValueFromKey(_entDataKey[3], entLoc.x);
    inFileLoader->getIntValueFromKey(_entDataKey[4], entLoc.y);
    _entLoc = entLoc;
    
    inFileLoader->getIntValueFromKey(_entDataKey[7], roomNumTest);//if the entity has a connect entity, let's add them to the Door Handler
    if(roomNumTest != BAD_DATA)//if the data is there...
    {
        inFileLoader->getIntValueFromKey(_entDataKey[8], entNum);
        _loadConnect.setPoints(roomNumTest, entNum);
        connectionToMake = true;
    }
    inFileLoader->getIntValueFromKey(_entDataKey[5], roomNumTest);//if the entity has a child entity, let's add them to the Door Handler
    if(roomNumTest != BAD_DATA)//if the data is there...
    {
        inFileLoader->getIntValueFromKey(_entDataKey[6], entNum);
        _loadChild.setPoints(roomNumTest, entNum);
        connectionToMake = true;
    }
    
    if(connectionToMake)
        _theGame->theDoorHand.addLoadingEnt(this);
}

void CSEntity::dataKeysInit(void)
{
    _entDataKey.push_back("EntNum");
    _entDataKey.push_back("EntType");
    _entDataKey.push_back("EntReg");
    _entDataKey.push_back("EntLocX");
    _entDataKey.push_back("EntLocY");
    _entDataKey.push_back("ChRoom");
    _entDataKey.push_back("ChEnt");
    _entDataKey.push_back("CoRoom");
    _entDataKey.push_back("CoEnt");
}


#pragma mark -
#pragma mark Setters

void CSEntity::setWasMoved(bool inState)
{
    _wasMoved = inState;
}

void CSEntity::setChar(char inChar)
{
    _entChar = inChar;
}

void CSEntity::setNum(int inEntNum)
{
    _entNum = inEntNum;
}

void CSEntity::setLoc(CSPoint *inLoc)
{
    _entLoc = *inLoc;
}

void CSEntity::setParent(CSEntity *inParent)
{
    _parentEnt = inParent;
    
    if(inParent != nullptr)
        if(_parentEnt->getChild() != this)
            _parentEnt->setChild(this);
}

void CSEntity::setChild(CSEntity *inChild)
{
    _childEnt = inChild;
    _loadChild.setPoints(-BAD_DATA, -BAD_DATA);
    
    if(inChild != nullptr)
        if(_childEnt->getParent() != this)//avoids infinite loop
            _childEnt->setParent(this);
    
    if(_entityType == ENT_DOOR)
        inChild = nullptr;
}

void CSEntity::setConnect(CSEntity *inConnect)
{
    _connect = inConnect;
    _loadConnect.setPoints(-BAD_DATA, -BAD_DATA);
    
    if(inConnect != nullptr)
        if(_connect->getConnect() != this)//avoids infinite loop
            inConnect->setConnect(this);
}

void CSEntity::setOwner(CSRoom *inOwner)
{
    _owner = inOwner;
}


#pragma mark -
#pragma mark Doers

void CSEntity::slideEntity(CSPoint inVector)
{
    CSPoint newLoc = _entLoc + inVector;
    bool    slideUp = false;
    
    //if we have a parent, pass this up the chain to them. If we don't set our loc, then go down the chain, and move our child on the same vector we were moved
    if(_parentEnt != nullptr)
    {
        if(!_parentEnt->getWasMoved())
            slideUp = true;
    }
    
    if(slideUp)
        _parentEnt->slideEntity(inVector);
    else
    {
        _wasMoved = true;
        setLoc(&newLoc);
        
        if(_childEnt != nullptr)
            _childEnt->slideEntity(inVector);
    }
}

bool CSEntity::slideDoor(CSPoint inVector)
{
    bool    goodLoc = true;
    CSAxis  wallAxis;
    CSPoint newLoc = (_entLoc + inVector);
    
    list<CSEntity*>            *roomEntities;
    list<CSEntity*>::iterator  listIter;
    
    wallAxis.setAxisFromWall(_entityRegion);
    _wasMoved = true;
    
    //if the door is attempting to be slid along the perp axis, away from the wall...
    if(inVector.getAxisPoint(wallAxis.getPerpAxis()) != 0)
    {
        if(!_owner->slideWall(_entityRegion, inVector.getAxisPoint(wallAxis.getPerpAxis())))
            return false;
        
        //loop through all of the moved wall's doors, see if they can be slid
        roomEntities = _owner->getEntities();
        for(listIter = roomEntities->begin(); listIter != roomEntities->end(); listIter++)
            if((*listIter)->checkForRegion(_entityRegion) && !(*listIter)->getWasMoved())//if it's an entity that should be moved, but hasn't yet been
            {
                if((*listIter)->getType() == ENT_DOOR)
                {
                    goodLoc = (*listIter)->slideDoor(inVector);//we try to slide it. If we can't...
                    if(!goodLoc)
                        break;
                }
                else//if it's not a door (like it's anchored to the wall that's being moved, e.g.: a room num)
                    (*listIter)->slideEntity(inVector);
            }
        
        //reset all entities' _wasMoved to false;
        for(listIter = roomEntities->begin(); listIter != roomEntities->end(); listIter++)
            (*listIter)->setWasMoved(false);
    }//if we come out the other side of this if, then this slide worked so far
    
    //if the door is attempting to be slid along the para axis, within the wall...
    if(inVector.getAxisPoint(wallAxis.dim) != 0)
        goodLoc = _owner->isWallPointFree(&newLoc, _entityRegion, this);//check that newDoorLoc is in free-wall. We're okay with with hallway walls, since we'll necessarily be checking a spot for the door that's in a wall.
    
    //if the intended location is within any free-wall range, we set ourselves to it and return that we were successful
    if(goodLoc)
        slideEntity(inVector);
    
    return goodLoc;
}

bool CSEntity::checkForRegion(entReg inReg)
{
    if(_entityRegion == REG_CORNER_TOP_LEFT)
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
        if(_entityRegion == inReg)
            return true;
        else
            return false;
}

void CSEntity::deleteEntity(void)
{
    if(_parentEnt!= nullptr)
        _parentEnt->setChild(nullptr);
    
    if(_childEnt!= nullptr)
        _childEnt->setParent(nullptr);
    
    if(_connect != nullptr)
        _connect->setConnect(nullptr);
    
    delete this;
}

bool CSEntity::updateEntity(void)//is overidden in CSCreature
{
    return false;
}

void CSEntity::activateAdjacent(void)
{
    _theGame->theBeHand.adjacentBehavior(this);
}

void CSEntity::activateOverlap(void)
{
}

string CSEntity::printEntityToFile(void)
{
    string outputString;
    
    outputString += _entDataKey[0];
    outputString += ":";
    outputString += (to_string(_entNum));
    outputString += "\n";
    outputString += _entDataKey[1];
    outputString += ":";
    outputString += to_string(_entityType);
    outputString += "\n";
    outputString += _entDataKey[2];
    outputString += ":";
    outputString += to_string(_entityRegion);
    outputString += "\n";
    outputString += _entDataKey[3];
    outputString += ":";
    outputString += to_string(_entLoc.x);
    outputString += "\n";
    outputString += _entDataKey[4];
    outputString += ":";
    outputString += to_string(_entLoc.y);
    outputString += "\n";
    
    if(_childEnt != nullptr)
    {
        outputString += _entDataKey[5];
        outputString += ":";
        outputString += to_string(_childEnt->getOwner()->getRoomNum());
        outputString += "\n";
        outputString += _entDataKey[6];
        outputString += ":";
        outputString += to_string(_childEnt->getNum());
        outputString += "\n";
    }
    
    if(_connect != nullptr)
    {
        outputString += _entDataKey[7];
        outputString += ":";
        outputString += to_string(_connect->getOwner()->getRoomNum());
        outputString += "\n";
        outputString += _entDataKey[8];
        outputString += ":";
        outputString += to_string(_connect->getNum());
        outputString += "\n";
    }
    
    return outputString;
}


#pragma mark -
#pragma mark Getters

bool CSEntity::getWasMoved(void)
{
    return _wasMoved;
}

int CSEntity::getNum(void)
{
    return _entNum;
}

char CSEntity::getChar(void)
{
    //set the char based on type
    switch(_entityType)
    {
        case ENT_ROOM_NUM:
            return _entChar;
        case ENT_CREATURE:
            return _entChar;
            
        default:
            return _theGame->theBeHand.getChar(_entityType);
    }
}

entType CSEntity::getType(void)
{
    return _entityType;
}

bool CSEntity::isPassable(void)
{
    return _theGame->theBeHand.isPassable(_entityType);
}

entReg CSEntity::getRegion(void)
{
    return _entityRegion;
}

CSPoint* CSEntity::getLoc(void)
{
    return &_entLoc;
}

CSPoint* CSEntity::getLoadConnect(void)
{
    return &_loadConnect;
}

CSPoint* CSEntity::getLoadChild(void)
{
    return &_loadChild;
}

CSEntity* CSEntity::getParent(void)
{
    return _parentEnt;
}

CSEntity* CSEntity::getChild(void)
{
    return _childEnt;
}

CSEntity* CSEntity::getConnect(void)
{
    return _connect;
}

CSRoom* CSEntity::getOwner(void)
{
    return _owner;
}

bool CSEntity::getIsPlayer(void)
{
    return false;
}






