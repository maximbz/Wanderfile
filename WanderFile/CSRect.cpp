//
//  CSRect.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 1/28/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSRect.hpp"

#pragma mark Constructors

CSRect::CSRect()
{
}

CSRect::CSRect(CSPoint inTopLeft, CSPoint inBotRight)
{
    topLeft = inTopLeft;
    botRight = inBotRight;
    
    calculateArea();
}

CSRect::CSRect(int inLeft, int inTop, int inRight, int inBot)
{
    topLeft.setPoints(inLeft, inTop);
    botRight.setPoints(inRight, inBot);
    
    calculateArea();
}


#pragma mark -
#pragma mark Setters

void CSRect::setPoints(int inLeft, int inTop, int inRight, int inBot)
{
    topLeft.setPoints(inLeft, inTop);
    botRight.setPoints(inRight, inBot);
    
    calculateArea();
}

void CSRect::setPoints(CSPoint inTopLeft, CSPoint inBotRight)
{
    topLeft = inTopLeft;
    botRight = inBotRight;
    
    calculateArea();
}

void CSRect::setTopLeft(CSPoint inTopLeft)
{
    topLeft = inTopLeft;
    
    calculateArea();
}

void CSRect::setTopLeft(int inLeft, int inTop)
{
    topLeft.setPoints(inLeft, inTop);
    
    calculateArea();
}

void CSRect::setBotRight(CSPoint inBotRight)
{
    botRight = inBotRight;
    
    calculateArea();
}

void CSRect::setBotRight(int inRight, int inBot)
{
    botRight.setPoints(inRight, inBot);
    
    calculateArea();
}

bool CSRect::setWallLoc(objReg inWall, int inLocPoint)
{
    //never let a rect's wall be set to the other side of its opposite wall--thus making the rect inverted.
    switch(inWall)
    {
        case REG_WALL_TOP:
            if(inLocPoint <= botRight.y)
            {
                topLeft.y = inLocPoint;
                calculateArea();
                return true;
            }
            else
                return false;
        case REG_WALL_LEFT:
            if(inLocPoint <= botRight.x)
            {
                topLeft.x = inLocPoint;
                calculateArea();
                return true;
            }
            else
                return false;
        case REG_WALL_RIGHT:
            if(inLocPoint >= topLeft.x)
            {
                botRight.x = inLocPoint;
                calculateArea();
                return true;
            }
            else
                return false;
        case REG_WALL_BOT:
            if(inLocPoint >= topLeft.y)
            {
                botRight.y = inLocPoint;
                calculateArea();
                return true;
            }
            else
                return false;
            
        default:
            printf("Error in CSRect: Attempting to set a wall loc other than the four possible walls.\n");
            return false;
    }
}

void CSRect::setWallRange(objReg inWall, CSRange inRange)
{
    switch(inWall)
    {
        case REG_WALL_TOP:
        case REG_WALL_BOT:
            topLeft.x = inRange.getMin();
            botRight.x = inRange.getMax();
            break;
        case REG_WALL_LEFT:
        case REG_WALL_RIGHT:
            topLeft.y = inRange.getMin();
            botRight.y = inRange.getMax();
            break;
            
        default:
            printf("Error in CSRect: Attempting to set a wall range other than the four possible walls.\n");
    }
    
    calculateArea();
}

void CSRect::setCorner(direction inDir, CSPoint inPoint)
{
    switch(inDir)
    {
        case DIR_UP_LEFT:
            topLeft = inPoint;
            break;
        case DIR_DOWN_RIGHT:
            botRight = inPoint;
            break;
            
        default:
            printf("Error in CSRect: Attempting to set a corner other than top-left or bottom-right.\n");
    }
    
    calculateArea();
}


#pragma mark -
#pragma mark Doers

void CSRect::calculateArea(void)
{
    //offset by 1 to track total width/height, instead of the diff between opposite walls
    _rectWidth = (botRight.x + 1) - topLeft.x;
    _rectHeight = (botRight.y + 1) - topLeft.y;
}

void CSRect::slideRect(CSPoint inVector)
{
    topLeft.x += inVector.x;
    botRight.x += inVector.x;
    
    topLeft.y += inVector.y;
    botRight.y += inVector.y;
}

bool CSRect::doesContain(CSPoint inPoint)
{
    return (inPoint.x >= topLeft.x && inPoint.x <= botRight.x) && (inPoint.y >= topLeft.y && inPoint.y <= botRight.y);
}

bool CSRect::doesRectContainWall(CSRect inRect, objReg inReg)
{
    int     wallLoc, clockWallLoc, countclockWallLoc;
    CSAxis  wallAxis;
    
    wallLoc = inRect.getWallLocPoint(inReg);
    clockWallLoc = inRect.getWallLocPoint(getClockWall(inReg));
    countclockWallLoc = inRect.getWallLocPoint(getCountclockWall(inReg));
    wallAxis.setAxisFromWall(inReg);
    
    if(getWallRange(getClockWall(inReg)).doesContain(wallLoc) &&//if inRect's left wall is in the range of this rect's top wall
       inRect.topLeft.getAxisPoint(wallAxis.dim) <= botRight.getAxisPoint(wallAxis.dim) &&//and inRect's topLeft wall is less than this rect's botRight wall
       inRect.botRight.getAxisPoint(wallAxis.dim) >= topLeft.getAxisPoint(wallAxis.dim))//and inRect's botRight wall is greater than this rect's topLeft wall
        return true;
    else
        return false;
}


#pragma mark -
#pragma mark Getters

int CSRect::getWidth(void)
{
    return _rectWidth;
}

int CSRect::getHeight(void)
{
    return _rectHeight;
}

int CSRect::getDim(axis inDim)
{
    switch(inDim)
    {
        case AXIS_HORIZ:
            return _rectWidth;
        case AXIS_VERT:
            return _rectHeight;
            
        default:
            printf("Error in CSRect: Attempting to get a dim other than horiz or vert.\n");
            return BAD_DATA;
    }
}

CSPoint CSRect::getCenterPoint(void)
{
    CSPoint outgoingPoint;
    
    outgoingPoint.x = topLeft.x + (_rectWidth / 2);
    outgoingPoint.y = topLeft.y + (_rectHeight / 2);
    
    return outgoingPoint;
}

CSPoint* CSRect::getCorner(direction inDir)
{
    switch(inDir)
    {
        case DIR_UP_LEFT:
            return &topLeft;
        case DIR_DOWN_RIGHT:
            return &botRight;
            
        default:
            printf("Error in CSRect: Attempting to get a corner's point other than top-left or bottom-right.\n");
            return nullptr;
    }
}

/*int CSRect::getWallSize(int inWall)
{
    switch(inWall)
    {
        case REG_WALL_LEFT:
        case REG_WALL_RIGHT:
            return _rectHeight;
        case REG_WALL_TOP:
        case REG_WALL_BOT:
            return _rectWidth;
        
        default:
            return BAD_DATA;
    }
}*/

int CSRect::getWallStartPoint(objReg inWall)
{
    switch(inWall)
    {
        case REG_WALL_LEFT:
        case REG_WALL_RIGHT:
            return topLeft.y;
        case REG_WALL_TOP:
        case REG_WALL_BOT:
            return topLeft.x;
            
        default:
            return BAD_DATA;
    }
}

int CSRect::getWallEndPoint(objReg inWall)
{
    switch(inWall)
    {
        case REG_WALL_LEFT:
        case REG_WALL_RIGHT:
            return botRight.y;
        case REG_WALL_TOP:
        case REG_WALL_BOT:
            return botRight.x;
            
        default:
            return BAD_DATA;
    }
}

int CSRect::getWallLocPoint(objReg inWall)
{
    switch(inWall)
    {
        case REG_WALL_LEFT:
            return topLeft.x;
        case REG_WALL_RIGHT:
            return botRight.x;
        case REG_WALL_TOP:
            return topLeft.y;
        case REG_WALL_BOT:
            return botRight.y;
            
        default:
            return BAD_DATA;
    }
}

CSRange CSRect::getWallRange(objReg inWall)
{
    CSRange outgoingRange;
    
    switch(inWall)
    {
        case REG_WALL_TOP:
        case REG_WALL_BOT:
            outgoingRange.setRange(topLeft.x,botRight.x);
            break;
        case REG_WALL_LEFT:
        case REG_WALL_RIGHT:
            outgoingRange.setRange(topLeft.y,botRight.y);
            break;
            
        default:
            outgoingRange.setRange(BAD_DATA,BAD_DATA);
    }
    
    return outgoingRange;
}

CSRange CSRect::getAxisRange(axis inAxis)
{
    objReg  regFromDim;
    
    switch(inAxis)
    {
        case AXIS_HORIZ:
            regFromDim = REG_WALL_TOP;
            break;
        case AXIS_VERT:
            regFromDim = REG_WALL_LEFT;
            break;
            
        default:
            regFromDim = REG_NULL;
    }
    
    return getWallRange(regFromDim);
}

#pragma mark -
#pragma mark Operators

CSRect& CSRect::operator=(const CSRect &inRect)
{
    topLeft = inRect.topLeft;
    botRight = inRect.botRight;
    
    return *this;
}

CSRect CSRect::operator+(const CSPoint &inVector)
{
    CSRect  tempRect = *this;
    
    tempRect.slideRect(inVector);
    tempRect.calculateArea();
    
    return tempRect;
}





