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


enum axis//axes/dimensions
{
    AXIS_HORIZ,
    AXIS_VERT,
    AXIS_NULL = -1
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
    REG_ROOM = 4,
    REG_CORNER_TOP_LEFT
};

//dir
const int   UP_LEFT = -1;
const int   DOWN_RIGHT = 1;
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
const int   ROOM_SIZE_MIN = 6;
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

inline axis getPerpAxis(axis incomingAxis)
{
    switch(incomingAxis)
    {
        case AXIS_HORIZ:
            return AXIS_VERT;
        case AXIS_VERT:
            return AXIS_HORIZ;
        default:
            printf("Error in WanderFile.h: Trying to return an axis other than horiz or vert.\n");
            return AXIS_NULL;
    }
}

inline axis getWallAxis(objReg incomingWall)
{
    switch(incomingWall)
    {
        case REG_WALL_TOP:
        case REG_WALL_BOT:
            return AXIS_HORIZ;
        case REG_WALL_LEFT:
        case REG_WALL_RIGHT:
            return AXIS_VERT;
        default:
            //printf("Error in WanderFile.h: Trying to return an axis other than horiz or vert.\n");
            return AXIS_NULL;
    }
}

inline int getWallDir(objReg incomingWall)
{
    switch(incomingWall)
    {
        case REG_WALL_LEFT:
        case REG_WALL_TOP:
            return UP_LEFT;
        case REG_WALL_RIGHT:
        case REG_WALL_BOT:
            return DOWN_RIGHT;
        default:
            printf("Error in WanderFile.h: Trying to return an direction other than up-left or down-right.\n");
            return BAD_DATA;
    }
}

inline objReg getFacingWall(objReg incomingWall)
{
    switch(incomingWall)
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
            return REG_NULL;
    }
}

inline objReg getClockwiseWall(objReg incomingWall)
{
    switch(incomingWall)
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
            return REG_NULL;
    }
}

inline objReg getRegionFromInt(int incomingInt)
{
    switch(incomingInt)
    {
        case REG_WALL_LEFT:
            return REG_WALL_LEFT;
        case REG_WALL_RIGHT:
            return REG_WALL_RIGHT;
        case REG_WALL_TOP:
            return REG_WALL_TOP;
        case REG_WALL_BOT:
            return REG_WALL_BOT;
            
        default:
            return REG_NULL;
    }
}

inline int getIntFromRegion(objReg incomingReg)
{
    switch(incomingReg)
    {
        case REG_WALL_LEFT:
            return REG_WALL_LEFT;
        case REG_WALL_RIGHT:
            return REG_WALL_RIGHT;
        case REG_WALL_TOP:
            return REG_WALL_TOP;
        case REG_WALL_BOT:
            return REG_WALL_BOT;
            
        default:
            return REG_NULL;
    }
}

#endif /* RandomRooms_h */
