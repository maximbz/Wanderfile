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


bool CSCreature::moveCreature(CSPoint *inVect)
{
    bool        roomChange = false;
    CSPoint     newPoint = _objectLoc;
    CSRoom      *nextOwner = _owner;//assume that after this move, creature will still be owned by the same room
    
    newPoint = newPoint + *inVect;
    
    if(!_owner->getRect()->doesRectContainPoint(&newPoint))//are we about to leave our current owner room?
    {
        roomChange = true;
        nextOwner = _owner->getObjectAtTile(&_objectLoc)->getConnect()->getOwner();//get the room connected to the door we're currently standing on
    }
    
    if(nextOwner->isTilePassable(&newPoint))//make sure the creaure can move to the tile it's trying to, whether they're leaving a room or not
    {
        if(roomChange)
            setOwner(nextOwner);
        
        slideObject(*inVect);
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
    CSPoint moveVect(0,0);
    
    if(_owner->getTheGame()->getBreakState())
        moveVect = moveVect;
    
    switch (_theRandHand->getNumber(&_moveDir))
    {
        case REG_WALL_LEFT:
            moveVect.x--;
            break;
        case REG_WALL_TOP:
            moveVect.y--;
            break;
        case REG_WALL_RIGHT:
            moveVect.x++;
            break;
        case REG_WALL_BOT:
            moveVect.y++;
            break;
            
        default:
            break;
    }
    
    return moveCreature(&moveVect);
}


bool CSCreature::getIsPlayer(void)
{
    return _player;
}
