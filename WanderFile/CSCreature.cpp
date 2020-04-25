//
//  CSCreature.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 4/3/19.
//  Copyright © 2019 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSCreature.hpp"
#include "CSRoom.hpp"
#include "CSGameState.hpp"

CSCreature::CSCreature(void)
{
    //this method is only used for creating the player, so no other initializaiton is required
    _entityType = ENT_CREATURE;
    _entityRegion = REG_ROOM;
}

CSCreature::CSCreature(CSPoint *inLoc, CSMonsterClass *inMonsterClass, CSRoom *inRoom, CSRandomHandler *inRandHand)
{
    _owner = inRoom;
    if(_owner != nullptr)
        _owner->addEntity(this);
    
    setChar(inMonsterClass->getChar());
    _hp = inMonsterClass->getHP();
    _atk = inMonsterClass->getAtk();
    _ac = inMonsterClass->getAC();
    _xp = inMonsterClass->getXP();
    _name = inMonsterClass->getName();
    _entLoc = *inLoc;
    
    _theRandHand = inRandHand;
    
    initCreature();
}

CSCreature::CSCreature(CSRoom *inRoom, CSRandomHandler *inRandHand, CSFileLoader *inFileLoader)//load entity from file
{
    int     entTypeAsInt, regAsInt;
    string  charAsString;
    CSPoint entLoc;
    
    _owner = inRoom;
    _theRandHand = inRandHand;
    
    initCreature();
    dataKeysInit();
    
    //set all the entity data
    inFileLoader->getIntValueFromKey(_entDataKey[0], _entNum);
    inFileLoader->getIntValueFromKey(_entDataKey[1], entTypeAsInt);
    _entityType = (entType)entTypeAsInt;
    inFileLoader->getIntValueFromKey(_entDataKey[2], regAsInt);
    _entityRegion = (entReg)regAsInt;
    inFileLoader->getIntValueFromKey(_entDataKey[3], entLoc.x);
    inFileLoader->getIntValueFromKey(_entDataKey[4], entLoc.y);
    _entLoc = entLoc;
    
    //set all the creature data
    inFileLoader->getIntValueFromKey(_creatureDataKey[0], _hp);
    inFileLoader->getIntValueFromKey(_creatureDataKey[1], _atk);
    inFileLoader->getIntValueFromKey(_creatureDataKey[2], _ac);
    inFileLoader->getIntValueFromKey(_creatureDataKey[3], _xp);
    inFileLoader->getStringValueFromKey(_creatureDataKey[4], _name);
    inFileLoader->getStringValueFromKey(_creatureDataKey[5], charAsString);
    _entChar = charAsString[0];
}

void CSCreature::initCreature(void)
{
    _player = false;
    _entityType = ENT_CREATURE;
    _entityRegion = REG_ROOM;
    
    _creatureDataKey.push_back("HP");
    _creatureDataKey.push_back("Atk");
    _creatureDataKey.push_back("AC");
    _creatureDataKey.push_back("XP");
    _creatureDataKey.push_back("Name");
    _creatureDataKey.push_back("Char");
    
    _moveDir.setRandType(RAND_MONSTER);
    _moveDir.setRangeMax(REG_WALL_LEFT);
    _moveDir.setRangeMin(REG_WALL_BOT);
    _theRandHand->addRandomRange(_moveDir);
}


#pragma mark -
#pragma mark Setters

void CSCreature::setIsPlayer(bool inPlayer)
{
    _player = inPlayer;
    
    if(_player)
    {
        setChar(PLAYER_CHAR);
        _name = "Player";
        _hp = 10;
        _atk = 1;
        _ac = 10;
        _xp = 0;
    }
}


#pragma mark -
#pragma mark Doers

bool CSCreature::moveCreature(entReg inReg)
{
    bool        roomChange = false;
    CSPoint     newPoint(&_entLoc, inReg), regVector(inReg);
    CSRoom      *nextOwner = _owner;//assume that after this move, creature will still be owned by the same room
    
    list<CSEntity *>::iterator   entIter;
    
    if(!_owner->getRect()->doesRectContainPoint(&newPoint))//are we about to leave our current owner room?
        for(entIter = _owner->getEntities()->begin(); entIter != _owner->getEntities()->end(); entIter++)
        {
            //in the extremely rare case that we're standing on a hall of length 1, both doors will be in the same locaiton, so we need to make sure we're passing the creature off to the correct connecting door, so we compare against the incoming region
            if((*entIter)->getType() == ENT_DOOR && (*entIter)->getRegion() == inReg && *((*entIter)->getLoc()) == _entLoc)
                nextOwner = (*entIter)->getConnect()->getOwner();//get the room connected to the door we're currently standing on
        }
    
    if(nextOwner->isTilePassable(&newPoint))//make sure the creaure can move to the tile it's trying to, whether they're leaving a room or not
    {
        if(_owner != nextOwner)
        {
            nextOwner->addEntity(this);
            roomChange = true;
        }
        
        slideEntity(regVector);
    }
    
    return roomChange;
}

string CSCreature::printEntityToFile(void)
{
    string outputString;
    
    outputString += CSEntity::printEntityToFile();
    
    //creature specific stuff
    outputString += _creatureDataKey[0];
    outputString += ":";
    outputString += to_string(_hp);
    outputString += "\n";
    outputString += _creatureDataKey[1];
    outputString += ":";
    outputString += to_string(_atk);
    outputString += "\n";
    outputString += _creatureDataKey[2];
    outputString += ":";
    outputString += to_string(_ac);
    outputString += "\n";
    outputString += _creatureDataKey[3];
    outputString += ":";
    outputString += to_string(_xp);
    outputString += "\n";
    outputString += _creatureDataKey[4];
    outputString += ":";
    outputString += _name;
    outputString += "\n";
    outputString += _creatureDataKey[5];
    outputString += ":";
    outputString += _entChar;
    outputString += "\n";

    return outputString;
}

void CSCreature::changeHP(int inHPChange)
{
    _hp += inHPChange;
    
    if(_hp <= 0)
        killCreature();
}

void CSCreature::killCreature(void)
{
    //room->delete entity(self)
    
    //player->addXP(_xp)
}

bool CSCreature::updateEntity(void)
{
    //AI & creature behavior goes here!
    //it suuure does...
    
    return moveCreature((entReg)_theRandHand->getNumber(&_moveDir));
}


#pragma mark -
#pragma mark Getters

bool CSCreature::getIsPlayer(void)
{
    return _player;
}
