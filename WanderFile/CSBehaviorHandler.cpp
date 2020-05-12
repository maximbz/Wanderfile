//
//  CSBehaviorHandler.cpp
//  WanderFile
//
//  Created by Maxim Boschert Zielsdorf on 4/30/20.
//  Copyright © 2020 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSBehaviorHandler.hpp"

CSBehaviorHandler::CSBehaviorHandler(void)
{
    
}


char CSBehaviorHandler::getChar(entType inType)
{
    //set the char based on type
    switch(inType)
    {
        case ENT_DOOR:
            return OPEN_DOOR_CHAR;
        case ENT_STAIRS_UP:
            return STAIRS_UP_CHAR;
        case ENT_STAIRS_DOWN:
            return STAIRS_DOWN_CHAR;
        case ENT_TREASURE:
            return CHEST_CHAR;
            
        default:
            return EMPTY_CHAR;
    }
}

bool CSBehaviorHandler::isPassable(entType inType)
{
    switch(inType)
    {
        case ENT_ROOM_NUM:
            return true;
        case ENT_CREATURE:
            return false;
        case ENT_DOOR:
            return true;
        case ENT_STAIRS_UP:
            return true;
        case ENT_STAIRS_DOWN:
            return true;
        case ENT_TREASURE:
            return false;
        
        default:
            return true;
    }
}
