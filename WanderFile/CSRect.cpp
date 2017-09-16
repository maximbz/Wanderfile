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

void CSRect::setWallLoc(int incomingWall, int incomingLocPoint)
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
    }
    
    calculateArea();
}

void CSRect::setWallRange(int incomingWall, CSRange incomingRange)
{
    switch(incomingWall)
    {
        case REG_WALL_TOP:
        case REG_WALL_BOT:
            topLeft.x = incomingRange.min;
            botRight.x = incomingRange.max;
            break;
        case REG_WALL_LEFT:
        case REG_WALL_RIGHT:
            topLeft.y = incomingRange.min;
            botRight.y = incomingRange.max;
            break;
    }
    
    calculateArea();
}

void CSRect::setCorner(int incomingDirection, CSPoint incomingPoint)
{
    switch(incomingDirection)
    {
        case UP_LEFT:
            topLeft = incomingPoint;
            break;
        case DOWN_RIGHT:
            botRight = incomingPoint;
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

CSPoint* CSRect::getCorner(int incomingDirection)
{
    switch(incomingDirection)
    {
        case UP_LEFT:
            return &topLeft;
        case DOWN_RIGHT:
            return &botRight;
        default:
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

int CSRect::getWallStartPoint(int incomingWall)
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

int CSRect::getWallEndPoint(int incomingWall)
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

int CSRect::getWallLocPoint(int incomingWall)
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

CSRange CSRect::getWallRange(int incomingWall)
{
    CSRange outgoingRange;
    
    switch (incomingWall)
    {
        case REG_WALL_TOP:
        case REG_WALL_BOT:
            outgoingRange.min = topLeft.x;
            outgoingRange.max = botRight.x;
            break;
        case REG_WALL_LEFT:
        case REG_WALL_RIGHT:
            outgoingRange.min = topLeft.y;
            outgoingRange.max = botRight.y;
            break;
        default:
            outgoingRange.min = BAD_DATA;
            outgoingRange.max = BAD_DATA;
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





