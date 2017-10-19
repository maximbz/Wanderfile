//
//  CSLine.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 10/17/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSLine.hpp"

CSLine::CSLine()
{
    _start = 0;
    _end = 0;
    _perpLoc = 0;
}

CSLine::CSLine(int inStart, int inEnd)
{
    _start = inStart;
    _end = inEnd;
    _perpLoc = 0;
}

CSLine::CSLine(int inStart, int inEnd, int inPerpLoc)
{
    _start = inStart;
    _end = inEnd;
    _perpLoc = inPerpLoc;
}


void CSLine::setLine(int inStart, int inEnd)
{
    _start = inStart;
    _end = inEnd;
}

void CSLine::setLine(int inStart, int inEnd, int inPerpLoc)
{
    _start = inStart;
    _end = inEnd;
    _perpLoc = inPerpLoc;
}

void CSLine::setStart(int inStart)
{
    _start = inStart;
}

void CSLine::setEnd(int inEnd)
{
    _end = inEnd;
}

void CSLine::setPerpLoc(int inPerpLoc)
{
    _perpLoc = inPerpLoc;
}


bool CSLine::doesContain(int inPoint)
{
    if(_start < _end)
        return inPoint >= _start && inPoint <= _end;
    else
        return inPoint >= _end && inPoint <= _start;
}

bool CSLine::isEndGreaterThanStart(void)
{
    return _end >= _start;
}

int CSLine::getSize(void)
{
    if(_start < _end)
        return _end - _start;
    else
        return _start - _end;
}


int CSLine::getStart(void)
{
    return _start;
}

int CSLine::getEnd(void)
{
    return _end;
}

int CSLine::getPerpLoc(void)
{
    return _perpLoc;
}




