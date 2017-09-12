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

CSRoomProx::CSRoomProx(CSRoom *incomingRoom, CSRange incomingRange, int incomingDistance)
{
    _nearbyRoom = incomingRoom;
    _roomOverlap = incomingRange;
    _distance = incomingDistance;
}


#pragma mark -
#pragma mark Setters

void CSRoomProx::setNearbyRoom(CSRoom *incomingRoom)
{
    _nearbyRoom = incomingRoom;
}

void CSRoomProx::setRoomOverlap(CSRange incomingRange)
{
    _roomOverlap = incomingRange;
}

void CSRoomProx::setRoomOverlap(int incomingMin, int incomingMax)
{
    _roomOverlap.min = incomingMin;
    _roomOverlap.max = incomingMax;
}

void CSRoomProx::setRoomOverlapMin(int incomingMin)
{
    _roomOverlap.min = incomingMin;
}

void CSRoomProx::setRoomOverlapMax(int incomingMax)
{
    _roomOverlap.max = incomingMax;
}

void CSRoomProx::setDistance(int incomingDistance)
{
    _distance = incomingDistance;
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
