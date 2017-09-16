//
//  CSPoint.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 11/25/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSPoint.hpp"

CSPoint::CSPoint(){}

CSPoint::CSPoint(int incomingX, int incomingY)
{
    x = incomingX;
    y = incomingY;
}


void CSPoint::setAxisPoint(axis incomingAxis, int incomingPoint)
{
    if(incomingAxis == AXIS_HORIZ)
        x = incomingPoint;
    else if(incomingAxis == AXIS_VERT)
        y = incomingPoint;
    else
        printf("Error in CSPoint: Attempting to access dimension other than X or Y.\n");
}

void CSPoint::setPoints(int incomingXPoint, int incomingYPoint)
{
    x = incomingXPoint;
    y = incomingYPoint;
}


void CSPoint::slidePoint(int incomingXPoint, int incomingYPoint)
{
    x += incomingXPoint;
    y += incomingYPoint;
}

void CSPoint::slidePoint(CSPoint incomingVector)
{
    x += incomingVector.x;
    y += incomingVector.y;
}

int CSPoint::getAxisPoint(axis incomingAxis)
{
    if(incomingAxis == AXIS_HORIZ)
        return x;
    else if(incomingAxis == AXIS_VERT)
        return y;
    else
    {
        printf("Error in CSPoint: Attempting to access dimension other than X or Y.\n");
        return BAD_DATA;//error
    }
}


//operator overloads!

CSPoint& CSPoint::operator=(const CSPoint &incomingPoint)
{
    x = incomingPoint.x;
    y = incomingPoint.y;
    
    return *this;
}

bool CSPoint::operator==(const CSPoint &incomingCoord)
{
    return ((x == incomingCoord.x) && (y == incomingCoord.y));
}

bool CSPoint::operator!=(const CSPoint &incomingCoord)
{
    return ((x != incomingCoord.x) || (y != incomingCoord.y));
}

//both x and y must be > or < etc for the entire CSPoint to be > or < etc
bool CSPoint::operator>(const CSPoint &incomingCoord)
{
    return ((x > incomingCoord.x) || (y > incomingCoord.y));
}

bool CSPoint::operator>=(const CSPoint &incomingCoord)
{
    return ((x >= incomingCoord.x) || (y >= incomingCoord.y));
}

bool CSPoint::operator<(const CSPoint &incomingCoord)
{
    return ((x < incomingCoord.x) || (y < incomingCoord.y));
}

bool CSPoint::operator<=(const CSPoint &incomingCoord)
{
    return ((x <= incomingCoord.x) || (y <= incomingCoord.y));
}


//math overloads

//mostly just for inverting with "* -1"
CSPoint CSPoint::operator*(int incomingInt)
{
    CSPoint outgoingPoint;
    
    outgoingPoint.x = x * incomingInt;
    outgoingPoint.y = y * incomingInt;
    
    return outgoingPoint;
}

CSPoint CSPoint::operator+(CSPoint &incomingPoint)
{
    CSPoint outgoingPoint;
    
    outgoingPoint.x = x + incomingPoint.x;
    outgoingPoint.y = y + incomingPoint.y;
    
    return outgoingPoint;
}

CSPoint CSPoint::operator-(CSPoint &incomingPoint)
{
    CSPoint outgoingPoint;
    
    outgoingPoint.x = x - incomingPoint.x;
    outgoingPoint.y = y - incomingPoint.y;
    
    return outgoingPoint;
}







