//
//  RandomRooms.h
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/1/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef RandomRooms_h
#define RandomRooms_h

#pragma mark Constants

enum randType//categories of random ranges and lists
{
    RAND_GAME,
    RAND_DUNGEON,
    RAND_ROOM
};

enum objType//dungeon object types
{
    OBJ_DOOR,
    OBJ_ROOM_NUM,
    OBJ_STAIRS_UP,
    OBJ_STAIRS_DOWN,
    OBJ_TREASURE
};

enum objReg//dungeon object regions
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
const int   WINDOW_BOUND_RIGHT = 150;
const int   WINDOW_BOUND_BOTTOM = 43;

const int   NUM_ROOM_WALLS = 4;
const int   NUM_DOORS_MIN = 20;
const float NUM_DOORS_LEVEL_RATE = 2;//rate at which doors increase per level
const float DOOR_GEN_SLOW_POINT = .7;
const int   ONE_DOOR_CHANCES = 30;
const int   NO_TREASURE_CHANCES = 30;

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


#pragma mark -
#pragma mark Inline Functions

inline objReg getFacingWall(objReg inWall)
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

inline objReg getClockWall(objReg inWall)
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

inline objReg getCountclockWall(objReg inWall)
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

inline objReg getNextReg(objReg inReg)//rotate to next region for next for iter
{
    int regCount;
    
    regCount = (int)inReg;
    regCount ++;
    
    if(regCount > (int)REG_WALL_BOT)//wraps wall-loop around
        regCount = (int)REG_WALL_LEFT;
    
    return (objReg)regCount;
}

#endif /* RandomRooms_h */

















