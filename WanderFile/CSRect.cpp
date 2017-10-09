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

CSRect::CSRect(CSPoint incomingTopLeft, CSPoint incomingBotRight)
{
    topLeft = incomingTopLeft;
    botRight = incomingBotRight;
    
    calculateArea();
}

CSRect::CSRect(int incomingLeft, int incomingTop, int incomingRight, int incomingBot)
{
    topLeft.setPoints(incomingLeft, incomingTop);
    botRight.setPoints(incomingRight, incomingBot);
    
    calculateArea();
}


#pragma mark -
#pragma mark Setters

void CSRect::setPoints(int incomingLeft, int incomingTop, int incomingRight, int incomingBot)
{
    topLeft.setPoints(incomingLeft, incomingTop);
    botRight.setPoints(incomingRight, incomingBot);
    
    calculateArea();
}

void CSRect::setPoints(CSPoint incomingTopLeft, CSPoint incomingBotRight)
{
    topLeft = incomingTopLeft;
    botRight = incomingBotRight;
    
    calculateArea();
}

void CSRect::setTopLeft(CSPoint incomingTopLeft)
{
    topLeft = incomingTopLeft;
    
    calculateArea();
}

void CSRect::setTopLeft(int incomingLeft, int incomingTop)
{
    topLeft.setPoints(incomingLeft, incomingTop);
    
    calculateArea();
}

void CSRect::setBotRight(CSPoint incomingBotRight)
{
    botRight = incomingBotRight;
    
    calculateArea();
}

void CSRect::setBotRight(int incomingRight, int incomingBot)
{
    botRight.setPoints(incomingRight, incomingBot);
    
    calculateArea();
}

void CSRect::setWallLoc(objReg incomingWall, int incomingLocPoint)
{
    switch(incomingWall)
    {
        case REG_WALL_TOP:
            topLeft.y = incomingLocPoint;
            break;
        case REG_WALL_LEFT:
            topLeft.x = incomingLocPoint;
            break;
        case REG_WALL_RIGHT:
            botRight.x = incomingLocPoint;
            break;
        case REG_WALL_BOT:
            botRight.y = incomingLocPoint;
            break;
        
        default:
            printf("Error in CSRect: Attempting to set a wall loc other than the four possible walls.\n");
    }
    
    calculateArea();
}

void CSRect::setWallRange(objReg incomingWall, CSRange incomingRange)
{
    switch(incomingWall)
    {
        case REG_WALL_TOP:
        case REG_WALL_BOT:
            topLeft.x = incomingRange.getMin();
            botRight.x = incomingRange.getMax();
            break;
        case REG_WALL_LEFT:
        case REG_WALL_RIGHT:
            topLeft.y = incomingRange.getMin();
            botRight.y = incomingRange.getMax();
            break;
            
        default:
            printf("Error in CSRect: Attempting to set a wall range other than the four possible walls.\n");
    }
    
    calculateArea();
}

void CSRect::setCorner(direction incomingDir, CSPoint incomingPoint)
{
    switch(incomingDir)
    {
        case DIR_UP_LEFT:
            topLeft = incomingPoint;
            break;
        case DIR_DOWN_RIGHT:
            botRight = incomingPoint;
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

void CSRect::slideRect(CSPoint incomingVector)
{
    topLeft.x += incomingVector.x;
    botRight.x += incomingVector.x;
    
    topLeft.y += incomingVector.y;
    botRight.y += incomingVector.y;
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

int CSRect::getDim(axis incomingDim)
{
    switch(incomingDim)
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

CSPoint* CSRect::getCorner(direction incomingDir)
{
    switch(incomingDir)
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

/*int CSRect::getWallSize(int incomingWall)
{
    switch(incomingWall)
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

int CSRect::getWallStartPoint(objReg incomingWall)
{
    switch(incomingWall)
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

int CSRect::getWallEndPoint(objReg incomingWall)
{
    switch(incomingWall)
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

int CSRect::getWallLocPoint(objReg incomingWall)
{
    switch(incomingWall)
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

CSRange CSRect::getWallRange(objReg incomingWall)
{
    CSRange outgoingRange;
    
    switch (incomingWall)
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

CSRect& CSRect::operator=(const CSRect &incomingRect)
{
    topLeft = incomingRect.topLeft;
    botRight = incomingRect.botRight;
    
    return *this;
}

CSRect CSRect::operator+(const CSPoint &incomingVector)
{
    CSRect  tempRect = *this;
    
    tempRect.slideRect(incomingVector);
    tempRect.calculateArea();
    
    return tempRect;
}





