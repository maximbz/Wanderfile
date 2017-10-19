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

void CSRect::setWallLoc(objReg inWall, int inLocPoint)
{
    switch(inWall)
    {
        case REG_WALL_TOP:
            topLeft.y = inLocPoint;
            break;
        case REG_WALL_LEFT:
            topLeft.x = inLocPoint;
            break;
        case REG_WALL_RIGHT:
            botRight.x = inLocPoint;
            break;
        case REG_WALL_BOT:
            botRight.y = inLocPoint;
            break;
        
        default:
            printf("Error in CSRect: Attempting to set a wall loc other than the four possible walls.\n");
    }
    
    calculateArea();
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
    
    switch (inWall)
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
            break;
    }
    
    return outgoingRange;
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





