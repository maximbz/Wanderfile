//
//  CSCreature.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 4/3/19.
//  Copyright © 2019 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSCreature.hpp"

CSCreature::CSCreature(void)
{
    
}

CSCreature::CSCreature(bool inPlayer, CSPoint inLoc)
{
    _player = inPlayer;
    _loc = inLoc;
}


bool CSCreature::moveCreature(CSPoint inVect)
{
    //check if we can move, based on what's in the tile we'd move into
    
    return true;
}


void CSCreature::setIsPlayer(bool inPlayer)
{
    _player = inPlayer;
}

void CSCreature::setLoc(CSPoint *inLoc)
{
    _loc = *inLoc;
}


bool CSCreature::getIsPlayer(void)
{
    return _player;
}

CSPoint* CSCreature::getLoc(void)
{
    return &_loc;
}
