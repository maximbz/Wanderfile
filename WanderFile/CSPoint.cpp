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


void CSPoint::setAxisPoint(int incomingDim, int incomingPoint)
{
    if(incomingDim == HORIZ)
        x = incomingPoint;
    else if(incomingDim == VERT)
        y = incomingPoint;
    else
        printf("Error: Attempting to access dimension other than X or Y.\n");
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

int CSPoint::getAxisPoint(int incomingDim)
{
    if(incomingDim == HORIZ)
        return x;
    else if(incomingDim == VERT)
        return y;
    else
    {
        printf("Error: Attempting to access dimension other than X or Y.\n");
        return -(LEVEL_BOUND_RIGHT + LEVEL_BOUND_BOTTOM);//error
    }
}


//operator overloads!
       
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







