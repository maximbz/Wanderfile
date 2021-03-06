//
//  RandomRooms.h
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/1/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef WanderFild_h
#define WanderFild_h

#include <stdio.h>

#pragma mark Constants

enum randType//categories of random ranges and lists
{
    RAND_GAME,
    RAND_DUNGEON,
    RAND_ROOM,
    RAND_MONSTER
};

enum entType//entity types
{
    ENT_DOOR,
    ENT_ROOM_NUM,
    ENT_STAIRS_UP,
    ENT_STAIRS_DOWN,
    ENT_TREASURE,
    ENT_CREATURE
};

enum entReg//entity regions
{
    REG_NULL = -1,
    REG_WALL_LEFT,
    REG_WALL_TOP,
    REG_WALL_RIGHT,
    REG_WALL_BOT,
    REG_ROOM_CORE,//rect centered around origin door; used for room gen
    REG_CORNER_TOP_LEFT,
    REG_ROOM//somewhere in the room, not in a wall
};

const int   BAD_DATA = -69420911;//nice
const int   RETURN_CODE_FALSE = 0;
const int   RETURN_CODE_TRUE = 1;
const int   RETURN_CODE_ABORT_GEN = 2;

const int   LEVEL_BOUND_RIGHT = 600;
const int   LEVEL_BOUND_BOTTOM = 600;
const int   WINDOW_BOUND_RIGHT = 79;
const int   WINDOW_BOUND_BOTTOM = 20;
const int   PLAYER_MOVE_BOUND_RIGHT = 45;
const int   PLAYER_MOVE_BOUND_BOTTOM = 9;
const int   MENU_BOUND_HEIGHT = 2;

const int   NUM_ROOM_WALLS = 4;
const int   NUM_DOORS_MIN = 20;
const float NUM_DOORS_LEVEL_RATE = 2;//rate at which doors increase per level
const float DOOR_GEN_SLOW_POINT = .7;
const int   ONE_DOOR_CHANCES = 30;
const int   NO_TREASURE_CHANCES = 30;
const float MONSTER_RATE = .3;
const int   MOVE_CHECK_DIST = 3;//how many rooms out monsters get to move/update
const bool  ROOM_NUMS = false;//set to true to turn on room numbers

//divide heights in half due to how ascii printing looks
const int   ROOM_SIZE_MIN = 2;
const int   ROOM_SIZE_MAX = 40;
const int   HALL_SIZE = 2;//+1 for total width/height
const int   HALL_LENGTH_MAX = 10;

const char  EMPTY_CHAR = ' ';
const char  WALL_CHAR = '#';
const char  FLOOR_CHAR = '.';
const char  PLAYER_CHAR = '@';
const char  OPEN_DOOR_CHAR = '.';
const char  CLOSED_DOOR_CHAR = '+';
const char  STAIRS_UP_CHAR = '<';
const char  STAIRS_DOWN_CHAR = '>';
const char  CHEST_CHAR = '=';

const char  FILE_TERM_CHAR = ';';
const char  FILE_DESCEND_CHAR = '/';
const char  FILE_ASCEND_CHAR = '.';
const char  FILE_NULL_CHAR = '-';


#pragma mark -
#pragma mark Inline Functions

inline entReg getFacingWall(entReg inWall)
{
    switch(inWall)
    {
        case REG_WALL_LEFT:
            return REG_WALL_RIGHT;
        case REG_WALL_RIGHT:
            return REG_WALL_LEFT;
        case REG_WALL_TOP:
            return REG_WALL_BOT;
        case REG_WALL_BOT:
            return REG_WALL_TOP;
            
        default:
            printf("Error in WanderFile.h: Trying to return a facing wall of a null wall.\n");
            return REG_NULL;
    }
}

inline entReg getClockWall(entReg inWall)
{
    switch(inWall)
    {
        case REG_WALL_LEFT:
            return REG_WALL_TOP;
        case REG_WALL_TOP:
            return REG_WALL_RIGHT;
        case REG_WALL_RIGHT:
            return REG_WALL_BOT;
        case REG_WALL_BOT:
            return REG_WALL_LEFT;
            
        default:
            printf("Error in WanderFile.h: Trying to return a clockwise wall of a null wall.\n");
            return REG_NULL;
    }
}

inline entReg getCountclockWall(entReg inWall)
{
    switch(inWall)
    {
        case REG_WALL_LEFT:
            return REG_WALL_BOT;
        case REG_WALL_TOP:
            return REG_WALL_LEFT;
        case REG_WALL_RIGHT:
            return REG_WALL_TOP;
        case REG_WALL_BOT:
            return REG_WALL_RIGHT;
            
        default:
            printf("Error in WanderFile.h: Trying to return a clockwise wall of a null wall.\n");
            return REG_NULL;
    }
}

inline entReg getRegFromVect(int inX, int inY)
{
    if(inX > 0)
        return REG_WALL_RIGHT;
    else if(inX < 0)
        return REG_WALL_LEFT;
    else//x == 0
    {
        if(inY > 0)
            return REG_WALL_BOT;
        else if(inY < 0)
            return REG_WALL_TOP;
    }
    
    return REG_NULL;
}

inline entReg getNextReg(entReg inReg)//rotate to next region for next for iter
{
    int regCount;
    
    regCount = (int)inReg;
    regCount ++;
    
    if(regCount > (int)REG_WALL_BOT)//wraps wall-loop around
        regCount = (int)REG_WALL_LEFT;
    
    return (entReg)regCount;
}

#endif /* RandomRooms_h */


