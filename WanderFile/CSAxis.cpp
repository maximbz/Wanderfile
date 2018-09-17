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

CSAxis::CSAxis(axis inAxis, direction inDir)
{
    dim = inAxis;
    dir = inDir;
}


#pragma mark -

void CSAxis::setAxis(axis inDim, orientation incOrientation)
{
    dim = inDim;
    
    if(incOrientation == PERPENDICULAR)//make dim perp
        dim = getPerpAxis();
}

void CSAxis::setAxisFromWall(objReg inWall)
{
    switch(inWall)
    {
        case REG_WALL_TOP:
            dim = AXIS_HORIZ;
            dir = DIR_UP_LEFT;
            break;
            
        case REG_WALL_BOT:
            dim = AXIS_HORIZ;
            dir = DIR_DOWN_RIGHT;
            break;
            
        case REG_WALL_LEFT:
            dim = AXIS_VERT;
            dir = DIR_UP_LEFT;
            break;
            
        case REG_WALL_RIGHT:
            dim = AXIS_VERT;
            dir = DIR_DOWN_RIGHT;
            break;
            
        default:
            //printf("Error in CSAxis: Trying to return an axis other than horiz or vert.\n");
            ;
    }
}

void CSAxis::setDir(direction inDir, orientation incOrientation)
{
    dir = inDir;
    
    if(incOrientation == PERPENDICULAR)//make dir opposing
        dir = getOpposingDir();
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

//Because, ascii characters use more pixels vertically, this cuts all vertical room gen by half to make rooms appear more square
int CSAxis::getAxisMod(int inLoc, orientation incOrientation)
{
    axis outDim;
    
    if(incOrientation == PARALLEL)
        outDim = dim;
    else
        outDim = getPerpAxis();
    
    switch(outDim)
    {
        case AXIS_HORIZ:
            return inLoc;
        case AXIS_VERT:
            return (inLoc / 2);
            
        default:
            printf("Error in CSAxis: Trying to modify an axis other than horiz or vert.\n");
            return AXIS_NULL;
    }
}

objReg CSAxis::getReg(void)
{
    if(dir == DIR_UP_LEFT)
    {
        if(dim == AXIS_VERT)
            return REG_WALL_LEFT;
        if(dim == AXIS_HORIZ)
            return REG_WALL_TOP;
    }
    
    if(dir == DIR_DOWN_RIGHT)
    {
        if(dim == AXIS_VERT)
            return REG_WALL_RIGHT;
        if(dim == AXIS_HORIZ)
            return REG_WALL_BOT;
    }
    
    printf("Error in CSAxis: Trying to return a region other than one of the four walls.");
    return REG_NULL;
}

axis CSAxis::getWallAxis(objReg inWall)
{
    switch(inWall)
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

direction CSAxis::getWallDir(objReg inWall)
{
    switch(inWall)
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
    switch(dir)
    {
        case DIR_UP_LEFT:
            return 1;
        case DIR_DOWN_RIGHT:
            return -1;
            
        default:
            printf("Error in CSAxis: Trying to return an direction other than up-left or down-right.\n");
            return 0;
    }
}






