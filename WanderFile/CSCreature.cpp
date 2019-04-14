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
