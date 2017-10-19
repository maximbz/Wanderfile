//
//  CSRoomProx.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 6/24/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSRoomProx.hpp"

#pragma mark Constructors

CSRoomProx::CSRoomProx()
{
}

CSRoomProx::CSRoomProx(CSRoom *inRoom, CSRange inRange, int inDistance)
{
    _nearbyRoom = inRoom;
    _roomOverlap = inRange;
    _distance = inDistance;
}


#pragma mark -
#pragma mark Setters

void CSRoomProx::setNearbyRoom(CSRoom *inRoom)
{
    _nearbyRoom = inRoom;
}

void CSRoomProx::setRoomOverlap(CSRange inRange)
{
    _roomOverlap = inRange;
}

void CSRoomProx::setRoomOverlap(int inMin, int inMax)
{
    _roomOverlap.setRange(inMin,inMax);
}

void CSRoomProx::setRoomOverlapMin(int inMin)
{
    _roomOverlap.setMin(inMin);
}

void CSRoomProx::setRoomOverlapMax(int inMax)
{
    _roomOverlap.setMax(inMax);
}

void CSRoomProx::setDistance(int inDistance)
{
    _distance = inDistance;
}


#pragma mark -
#pragma mark Getters

CSRoom* CSRoomProx::getNearbyRoom(void)
{
    return _nearbyRoom;
}

CSRange CSRoomProx::getRoomOverlap(void)
{
    return _roomOverlap;
}

int CSRoomProx::getDistance(void)
{
    return _distance;
}
