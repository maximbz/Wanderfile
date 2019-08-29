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
    initCreature();
}

/*CSCreature::CSCreature(bool inPlayer, CSPoint *inLoc)
{
    initCreature();
    _player = inPlayer;
    setChar(PLAYER_CHAR);
}*/

CSCreature::CSCreature(CSPoint *inLoc, CSMonsterClass *inMonsterClass, CSRoom *inRoom, CSRandomHandler *inRandHand)
{
    _owner = inRoom;
    
    initCreature();
    setChar(inMonsterClass->getChar());
    _hp = inMonsterClass->getHP();
    _atk = inMonsterClass->getAtk();
    _ac = inMonsterClass->getAC();
    _xp = inMonsterClass->getXP();
    _name = inMonsterClass->getName();
    _objectLoc = *inLoc;
    
    _theRandHand = inRandHand;
    _moveDir.setRandType(RAND_MONSTER);
    _moveDir.setRangeMax(REG_WALL_LEFT);
    _moveDir.setRangeMin(REG_WALL_BOT);
    _theRandHand->addRandomRange(_moveDir);
}

void CSCreature::initCreature(void)
{
    _player = false;
    _objectType = OBJ_CREATURE;
    _objectRegion = REG_ROOM;
}


bool CSCreature::moveCreature(objReg inReg)
{
    bool        roomChange = false;
    CSPoint     newPoint(&_objectLoc, inReg), regVector(inReg);
    CSRoom      *nextOwner = _owner;//assume that after this move, creature will still be owned by the same room
    
    list<CSDungObj *>::iterator   objIter;
    
    if(!_owner->getRect()->doesRectContainPoint(&newPoint))//are we about to leave our current owner room?
        for(objIter = _owner->getObjects()->begin(); objIter != _owner->getObjects()->end(); objIter++)
        {
            //in the extremely rare case that we're standing on a hall of length 1, both doors will be in the same locaiton, so we need to make sure we're passing the creature off to the correct connecting door, so we compare against the incoming region
            if((*objIter)->getType() == OBJ_DOOR && (*objIter)->getRegion() == inReg && *((*objIter)->getLoc()) == _objectLoc)
                nextOwner = (*objIter)->getConnect()->getOwner();//get the room connected to the door we're currently standing on
        }
    
    if(nextOwner->isTilePassable(&newPoint))//make sure the creaure can move to the tile it's trying to, whether they're leaving a room or not
    {
        if(_owner != nextOwner)
        {
            setOwner(nextOwner);
            roomChange = true;
        }
        
        slideObject(regVector);
    }
    
    return roomChange;
}


void CSCreature::setIsPlayer(bool inPlayer)
{
    _player = inPlayer;
    
    if(_player)
        setChar(PLAYER_CHAR);
}

void CSCreature::setNullOwner(void)//for pulling the player out of whatever room it's in before deleting the dungeon (and thus all the objects within it)
{
    _owner = nullptr;
}




void CSCreature::changeHP(int inHPChange)
{
    _hp += inHPChange;
    
    if(_hp <= 0)
        killCreature();
}

void CSCreature::killCreature(void)
{
    
}

bool CSCreature::updateObject(void)
{
    //AI & creature behavior goes here!
    //it suuure does...
    
    return moveCreature((objReg)_theRandHand->getNumber(&_moveDir));
}


bool CSCreature::getIsPlayer(void)
{
    return _player;
}
