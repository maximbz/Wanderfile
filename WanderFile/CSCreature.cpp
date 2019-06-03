//
//  CSCreature.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 4/3/19.
//  Copyright © 2019 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSCreature.hpp"

CSCreature::CSCreature(void) : _creatureObj(OBJ_CREATURE, REG_ROOM, nullptr, nullptr, nullptr, nullptr)
{
    
}

CSCreature::CSCreature(bool inPlayer, CSPoint *inLoc) : _creatureObj(OBJ_CREATURE, REG_ROOM, inLoc, nullptr, nullptr, nullptr)
{
    _player = inPlayer;
    
    if(_player)
        _creatureObj.setChar(PLAYER_CHAR);
}

CSCreature::CSCreature(CSPoint *inLoc, CSMonsterClass *inMonsterClass) : _creatureObj(OBJ_CREATURE, REG_ROOM, inLoc, nullptr, nullptr, nullptr)
{
    _creatureObj.setChar(inMonsterClass->getChar());
    _hp = inMonsterClass->getHP();
    _atk = inMonsterClass->getAtk();
    _ac = inMonsterClass->getAC();
    _xp = inMonsterClass->getXP();
    _name = inMonsterClass->getName();
}


void CSCreature::moveCreature(CSPoint *inVect)
{
    _creatureObj.slideObject(*inVect);
}


void CSCreature::setIsPlayer(bool inPlayer)
{
    _player = inPlayer;
    
    if(_player)
        _creatureObj.setChar(PLAYER_CHAR);
}

void CSCreature::setLoc(CSPoint *inLoc)
{
    _creatureObj.setLoc(*inLoc);
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


bool CSCreature::getIsPlayer(void)
{
    return _player;
}

CSPoint* CSCreature::getLoc(void)
{
    return _creatureObj.getLoc();
}

CSDungObj* CSCreature::getCreatureObj(void)
{
    return &_creatureObj;
}
