//
//  CSGameState.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 1/28/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSGameState.hpp"
#include "WanderFile.h"

CSGameState::CSGameState()
{
    _printRoomNums = true;//false
    _breakForDebug = false;
    
    _gameWindow.setPoints((LEVEL_BOUND_RIGHT / 2) - (WINDOW_BOUND_RIGHT / 2), (LEVEL_BOUND_BOTTOM / 2) - (WINDOW_BOUND_BOTTOM / 2), (LEVEL_BOUND_RIGHT / 2) + (WINDOW_BOUND_RIGHT / 2), (LEVEL_BOUND_BOTTOM / 2) + (WINDOW_BOUND_BOTTOM / 2));
}

void CSGameState::setGameWindow(CSRect incomingRect)
{
    _gameWindow = incomingRect;
}


void CSGameState::slideGameWindow(int incomingXDist, int incomingYDist)
{
    if((_gameWindow.topLeft.x > 1 && incomingXDist < 0) || (_gameWindow.botRight.x < LEVEL_BOUND_RIGHT && incomingXDist > 0))
    {
        _gameWindow.topLeft.x += incomingXDist;
        _gameWindow.botRight.x += incomingXDist;
    }
    
    if((_gameWindow.topLeft.y > 1 && incomingYDist < 0) || (_gameWindow.botRight.y < LEVEL_BOUND_BOTTOM && incomingYDist > 0))
    {
        _gameWindow.topLeft.y += incomingYDist;
        _gameWindow.botRight.y += incomingYDist;
    }
    
    printf("Game Window: %d, %d - %d, %d\n", _gameWindow.topLeft.x, _gameWindow.topLeft.y, _gameWindow.botRight.x, _gameWindow.botRight.y);
}

void CSGameState::centerGameWindow(CSPoint incomingPoint)
{
    bool    topLeftAnchor = true;
    
    //center on incoming point. If that puts gameWindow outside of LEVEL_BOUNDS, set gameWindow to LEVEL_BOUNDS
    
    _gameWindow.topLeft.x =  incomingPoint.x - (WINDOW_BOUND_RIGHT / 2);
    if(_gameWindow.topLeft.x < 0)
        _gameWindow.topLeft.x = 0;
    else if(_gameWindow.botRight.x > LEVEL_BOUND_RIGHT)
    {
        _gameWindow.botRight.x = LEVEL_BOUND_RIGHT;
        topLeftAnchor = false;
    }
    
    _gameWindow.topLeft.y = incomingPoint.y - (WINDOW_BOUND_BOTTOM / 2);
    if(_gameWindow.topLeft.y < 0)
        _gameWindow.topLeft.y = 0;
    else if(_gameWindow.botRight.y > LEVEL_BOUND_BOTTOM)
    {
        _gameWindow.botRight.y = LEVEL_BOUND_BOTTOM;
        topLeftAnchor = false;
    }
    
    
    //adjust other corner based on the corner we set
    
    if(topLeftAnchor)
    {
        _gameWindow.botRight.x = _gameWindow.topLeft.x + WINDOW_BOUND_RIGHT;
        _gameWindow.botRight.y = _gameWindow.topLeft.y + WINDOW_BOUND_BOTTOM;
    }
    else
    {
        _gameWindow.topLeft.x = _gameWindow.botRight.x - WINDOW_BOUND_RIGHT;
        _gameWindow.topLeft.y = _gameWindow.botRight.y - WINDOW_BOUND_BOTTOM;
    }
}

void CSGameState::toggleRoomNums(void)
{
    _printRoomNums = !_printRoomNums;
}

void CSGameState::toggleBreak(void)
{
    _breakForDebug = !_breakForDebug;
}


CSRect CSGameState::getGameWindow(void)
{
    return _gameWindow;
}

bool CSGameState::getRoomNumsState(void)
{
    return _printRoomNums;
}

bool CSGameState::getBreakState(void)
{
    return _breakForDebug;
}
