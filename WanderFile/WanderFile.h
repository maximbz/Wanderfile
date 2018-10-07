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
    RAND_DUNGEON
};

enum objType//dungeon object types
{
    OBJ_DOOR,
    OBJ_ROOM_NUM
};

enum objReg//dungeon object regions
{
    REG_NULL = -1,
    REG_WALL_LEFT,
    REG_WALL_TOP,
    REG_WALL_RIGHT,
    REG_WALL_BOT,
    REG_ROOM_RECT_MIN,//rect centered around origin door; used for room gen
    REG_CORNER_TOP_LEFT,
};

const int   BAD_DATA = -69420911;//nice

const int   LEVEL_BOUND_RIGHT = 600;
const int   LEVEL_BOUND_BOTTOM = 600;
const int   WINDOW_BOUND_RIGHT = 150;
const int   WINDOW_BOUND_BOTTOM = 50;

const int   NUM_ROOMS = 300;
const int   NUM_ROOM_WALLS = 4;


/*const int   ROOM_SIZE_X_MIN = 5;
const int   ROOM_SIZE_X_MAX = 40;
const int   ROOM_SIZE_Y_MIN = 3;
const int   ROOM_SIZE_Y_MAX = 18;*/
//divide heights in half due to how ascii printing looks
const int   ROOM_SIZE_MIN = 2;
const int   ROOM_SIZE_MAX = 40;
const int   HALL_SIZE = 2;//+1 for total width/height
const int   HALL_LENGTH_MAX = 10;
const int   ROOM_DISTANCE_MIN = 5;

const char  WALL_CHAR = '#';
const char  FLOOR_CHAR = '.';
const char  PLAYER_CHAR = '@';
const char  OPEN_DOOR_CHAR = '.';
const char  CLOSED_DOOR_CHAR = '+';


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

inline objReg getRegFromInt(int inInt)
{
    switch(inInt)
    {
        case REG_ROOM_RECT_MIN:
            return REG_ROOM_RECT_MIN;
        case REG_WALL_LEFT:
            return REG_WALL_LEFT;
        case REG_WALL_RIGHT:
            return REG_WALL_RIGHT;
        case REG_WALL_TOP:
            return REG_WALL_TOP;
        case REG_WALL_BOT:
            return REG_WALL_BOT;
        case REG_CORNER_TOP_LEFT:
            return REG_CORNER_TOP_LEFT;
            
        default:
            return REG_NULL;
    }
}

inline int getIntFromReg(objReg inReg)
{
    switch(inReg)
    {
        case REG_ROOM_RECT_MIN:
            return REG_ROOM_RECT_MIN;
        case REG_WALL_LEFT:
            return REG_WALL_LEFT;
        case REG_WALL_RIGHT:
            return REG_WALL_RIGHT;
        case REG_WALL_TOP:
            return REG_WALL_TOP;
        case REG_WALL_BOT:
            return REG_WALL_BOT;
        case REG_CORNER_TOP_LEFT:
            return REG_CORNER_TOP_LEFT;
            
        default:
            return REG_NULL;
    }
}

inline objReg getNextReg(objReg inReg)//rotate to next region for next for iter
{
    int regCount;
    
    regCount = getIntFromReg(inReg);
    regCount ++;
    
    if(regCount > getIntFromReg(REG_WALL_BOT))//wraps wall-loop around
        regCount = getIntFromReg(REG_WALL_LEFT);
    
    return getRegFromInt(regCount);
}

#endif /* RandomRooms_h */

















