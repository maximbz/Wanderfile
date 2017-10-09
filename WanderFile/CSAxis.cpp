//
//  CSAxis.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 9/16/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSAxis.hpp"

CSAxis::CSAxis()
{
    dim = AXIS_NULL;
    dir = DIR_NULL;
}

CSAxis::CSAxis(axis incomingAxis, direction incomingDir)
{
    dim = incomingAxis;
    dir = incomingDir;
}


#pragma mark -

void CSAxis::setAxis(axis incomingDim, orientation incOrientation)
{
    dim = incomingDim;
    
    if(incOrientation == PERPENDICULAR)//make dim perp
        dim = getPerpAxis();
}

void CSAxis::setAxisFromWall(objReg incomingWall)
{
    switch(incomingWall)
    {
        case REG_WALL_TOP:
        case REG_WALL_BOT:
            dim = AXIS_HORIZ;
            break;
        case REG_WALL_LEFT:
        case REG_WALL_RIGHT:
            dim = AXIS_VERT;
            break;
            
        default:
            //printf("Error in CSAxis: Trying to return an axis other than horiz or vert.\n");
            ;
    }
}

void CSAxis::setDir(direction incomingDir, orientation incOrientation)
{
    dir = incomingDir;
    
    if(incOrientation == PERPENDICULAR)//make dir opposing
        dir = getOpposingDir();
}

void CSAxis::setDirFromWall(objReg incomingWall)
{
    switch(incomingWall)
    {
        case REG_WALL_LEFT:
        case REG_WALL_TOP:
            dir = DIR_UP_LEFT;
            break;
        case REG_WALL_RIGHT:
        case REG_WALL_BOT:
            dir = DIR_DOWN_RIGHT;
            break;
            
        default:
            //printf("Error in CSAxis: Trying to return a direction other than up-left or down-right.\n");
            ;
    }
}


#pragma mark -

axis CSAxis::getPerpAxis(void)
{
    switch(dim)
    {
        case AXIS_HORIZ:
            return AXIS_VERT;
        case AXIS_VERT:
            return AXIS_HORIZ;
        default:
            printf("Error in CSAxis: Trying to return an axis other than horiz or vert.\n");
            return AXIS_NULL;
    }
}

int CSAxis::getAxisMod(int incomingLoc, orientation incOrientation)
{
    axis outDim;
    
    if(incOrientation == PARALLEL)
        outDim = dim;
    else
        outDim = getPerpAxis();
    
    switch(outDim)
    {
        case AXIS_HORIZ:
            return incomingLoc;
        case AXIS_VERT:
            return (incomingLoc / 2);
            
        default:
            printf("Error in CSAxis: Trying to modify an axis other than horiz or vert.\n");
            return AXIS_NULL;
    }
}

axis CSAxis::getWallAxis(objReg incomingWall)
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
            //printf("Error in CSAxis: Trying to return an axis other than horiz or vert.\n");
            return AXIS_NULL;
    }
}

direction CSAxis::getWallDir(objReg incomingWall)
{
    switch(incomingWall)
    {
        case REG_WALL_LEFT:
        case REG_WALL_TOP:
            return DIR_UP_LEFT;
        case REG_WALL_RIGHT:
        case REG_WALL_BOT:
            return DIR_DOWN_RIGHT;
        default:
            printf("Error in CSAxis: Trying to return an direction other than up-left or down-right.\n");
            return DIR_NULL;
    }
}


#pragma mark -

direction CSAxis::getOpposingDir(void)
{
    switch(dir)
    {
        case DIR_UP_LEFT:
            return DIR_DOWN_RIGHT;
        case DIR_DOWN_RIGHT:
            return DIR_UP_LEFT;
            
        default:
            printf("Error in CSAxis: Trying to return an direction other than up-left or down-right.\n");
            return DIR_NULL;
    }
}

int CSAxis::getDirectionOffset(void)
{
    switch(dir)
    {
        case DIR_UP_LEFT:
            return -1;
        case DIR_DOWN_RIGHT:
            return 1;
            
        default:
            printf("Error in CSAxis: Trying to return an direction other than up-left or down-right.\n");
            return 0;
    }
}

int CSAxis::getOpposingDirOffset(void)
{
    switch(getOpposingDir())
    {
        case DIR_UP_LEFT:
            return -1;
        case DIR_DOWN_RIGHT:
            return 1;
            
        default:
            printf("Error in CSAxis: Trying to return an direction other than up-left or down-right.\n");
            return 0;
    }
}






