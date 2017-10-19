//
//  CSPoint.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 11/25/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSPoint.hpp"

CSPoint::CSPoint(){}

CSPoint::CSPoint(int inX, int inY)
{
    x = inX;
    y = inY;
}


void CSPoint::setAxisPoint(axis inAxis, int inPoint)
{
    if(inAxis == AXIS_HORIZ)
        x = inPoint;
    else if(inAxis == AXIS_VERT)
        y = inPoint;
    else
        printf("Error in CSPoint: Attempting to access dimension other than X or Y.\n");
}

void CSPoint::setPoints(int inXPoint, int inYPoint)
{
    x = inXPoint;
    y = inYPoint;
}


void CSPoint::slidePoint(int inXPoint, int inYPoint)
{
    x += inXPoint;
    y += inYPoint;
}

void CSPoint::slidePoint(CSPoint inVector)
{
    x += inVector.x;
    y += inVector.y;
}

int CSPoint::getAxisPoint(axis inAxis)
{
    if(inAxis == AXIS_HORIZ)
        return x;
    else if(inAxis == AXIS_VERT)
        return y;
    else
    {
        printf("Error in CSPoint: Attempting to access dimension other than X or Y.\n");
        return BAD_DATA;//error
    }
}


//operator overloads!

CSPoint& CSPoint::operator=(const CSPoint &inPoint)
{
    x = inPoint.x;
    y = inPoint.y;
    
    return *this;
}

bool CSPoint::operator==(const CSPoint &inCoord)
{
    return ((x == inCoord.x) && (y == inCoord.y));
}

bool CSPoint::operator!=(const CSPoint &inCoord)
{
    return ((x != inCoord.x) || (y != inCoord.y));
}

//both x and y must be > or < etc for the entire CSPoint to be > or < etc
bool CSPoint::operator>(const CSPoint &inCoord)
{
    return ((x > inCoord.x) || (y > inCoord.y));
}

bool CSPoint::operator>=(const CSPoint &inCoord)
{
    return ((x >= inCoord.x) || (y >= inCoord.y));
}

bool CSPoint::operator<(const CSPoint &inCoord)
{
    return ((x < inCoord.x) || (y < inCoord.y));
}

bool CSPoint::operator<=(const CSPoint &inCoord)
{
    return ((x <= inCoord.x) || (y <= inCoord.y));
}


//math overloads

//mostly just for inverting with "* -1"
CSPoint CSPoint::operator*(int inInt)
{
    CSPoint outPoint;
    
    outPoint.x = x * inInt;
    outPoint.y = y * inInt;
    
    return outPoint;
}

CSPoint CSPoint::operator+(CSPoint &inPoint)
{
    CSPoint outPoint;
    
    outPoint.x = x + inPoint.x;
    outPoint.y = y + inPoint.y;
    
    return outPoint;
}

CSPoint CSPoint::operator-(CSPoint &inPoint)
{
    CSPoint outPoint;
    
    outPoint.x = x - inPoint.x;
    outPoint.y = y - inPoint.y;
    
    return outPoint;
}







