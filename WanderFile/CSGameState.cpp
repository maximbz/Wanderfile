//
//  CSGameState.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 1/28/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include "CSGameState.hpp"
#include "WanderFile.h"
#include "CSRange.hpp"

using namespace std;

CSGameState::CSGameState()
{
    _printRoomNums = true;//false
    _breakForDebug = false;
    
    _levelBounds.setPoints(0, 0, LEVEL_BOUND_RIGHT, LEVEL_BOUND_BOTTOM);
    
    _gameWindow.setPoints((LEVEL_BOUND_RIGHT / 2) - (WINDOW_BOUND_RIGHT / 2), (LEVEL_BOUND_BOTTOM / 2) - (WINDOW_BOUND_BOTTOM / 2), (LEVEL_BOUND_RIGHT / 2) + (WINDOW_BOUND_RIGHT / 2), (LEVEL_BOUND_BOTTOM / 2) + (WINDOW_BOUND_BOTTOM / 2));
    
    _theplayer.setIsPlayer(true);
    if(loadMonsterManual())
        printf("Monsters did not load. Is the file 'MonsterManual.txt' missing?\n");
}

void CSGameState::setGameWindow(CSRect inRect)
{
    _gameWindow = inRect;
}


int CSGameState::loadMonsterManual(void)
{
    bool            key, min;
    char            newChar = '!';
    int             loop, intFromStr, rangeMin = 1, rangeMax = 1, newHP = 0, newAtk = 0, newAC = 0, newXP = 0;
    ifstream        inputFile;
    string          inputString, keyString, valueString, newName;
    CSRange         newAppearing;
    CSMonsterClass  *newMonsterClass;
    
    vector<string >            fileData;
    vector<string >::iterator  vectIter;
    
    //open the file and check for errors
    inputFile.open("MonsterManual.txt");
    if(inputFile.fail())
    {
        perror(inputString.c_str());
        return 1;
    }
    
    //pull each line of the file into stringVect
    while(getline(inputFile, inputString))
        fileData.push_back(inputString);
    
    //each for loop is a line in fileData
    for(vectIter = fileData.begin(); vectIter != fileData.end(); vectIter++)
    {
        key = true;
        min = true;
        keyString = "";
        valueString = "";
        inputString = *vectIter;
        intFromStr = 0;
        
        //parse through fileData string and populate key and value strings
        for(loop = 0; loop < inputString.size(); loop++)
        {
            if(inputString[loop] == ';')//if we hit a semi-colon...
            {
                newMonsterClass = new CSMonsterClass(newChar, newHP, newAtk, newAC, newXP, newName, &newAppearing);//make a new monster
                _monsterManual.push_back(newMonsterClass);//complete the monster
                
                newChar = '!';
                rangeMin = 1;
                rangeMax = 1;
                continue;
            }
            else if(inputString[loop] != ':')//switch from key to value input
            {
                if(key)
                    keyString += inputString[loop];
                else
                    valueString += inputString[loop];
            }
            else
                key = false;
        }
        
        //parse through fileData string and populate this new monster
        stringstream    strToInt(valueString);
        strToInt >> intFromStr;
        
        if(keyString == "Char")
            newChar = valueString[0];
        else if(keyString == "HP")
            newHP = intFromStr;
        else if(keyString == "Atk")
            newAtk = intFromStr;
        else if(keyString == "AC")
            newAC = intFromStr;
        else if(keyString == "XP")
            newXP = intFromStr;
        else if(keyString == "Name")
            newName = valueString;
        else if(keyString == "Appear")
        {
            for(loop = 0; loop < valueString.size(); loop++)//loop through range (the content of valueString)
            {
                if(valueString[loop] == '-')//switch from determining min to determining max
                    min = false;
                else
                {
                    intFromStr = valueString[loop] - '0';//convert char to int by subtracting ascii offset
                    if(min)
                        rangeMin = intFromStr;
                    else
                        rangeMax = intFromStr;
                }
            }
            
            newAppearing.setRange(rangeMin, rangeMax);
        }
    }
    
    return 0;
}

void CSGameState::cleanUpGameState(void)
{
    list<CSMonsterClass *>::iterator    listIter = _monsterManual.begin();
    
    while(listIter != _monsterManual.end())
        listIter = _monsterManual.erase(listIter);//new iterator properly goes through the list, now with fewer entries
    
    _monsterManual.clear();
}

void CSGameState::slideGameWindow(CSPoint *inVect)
{
    int     loop;
    CSPoint dimVect;
    
    for(loop = AXIS_HORIZ; loop <= AXIS_VERT; loop++)
    {
        if((_gameWindow.topLeft.getAxisPoint((axis)loop) > 1 && inVect->getAxisPoint((axis)loop) < 0) ||
           (_gameWindow.botRight.getAxisPoint((axis)loop) < _levelBounds.botRight.getAxisPoint((axis)loop) && inVect->getAxisPoint((axis)loop) > 0))
        {
            dimVect.setAxisPoint((axis)loop, inVect->getAxisPoint((axis)loop));
            _gameWindow.slideRect(&dimVect);
        }
        
        dimVect.setPoints(0, 0);
    }
    
    printf("Game Window: %d, %d - %d, %d\n", _gameWindow.topLeft.x, _gameWindow.topLeft.y, _gameWindow.botRight.x, _gameWindow.botRight.y);
}

void CSGameState::centerGameWindow(CSPoint *inPoint)
{
    bool    topLeftAnchor = true;
    
    //center on in point. If that puts gameWindow outside of LEVEL_BOUNDS, set gameWindow to LEVEL_BOUNDS
    
    _gameWindow.topLeft.x =  inPoint->x - (WINDOW_BOUND_RIGHT / 2);
    if(_gameWindow.topLeft.x < 0)
        _gameWindow.topLeft.x = 0;
    else if(_gameWindow.botRight.x > _levelBounds.botRight.x)
    {
        _gameWindow.botRight.x = _levelBounds.botRight.x;
        topLeftAnchor = false;
    }
    
    _gameWindow.topLeft.y = inPoint->y - (WINDOW_BOUND_BOTTOM / 2);
    if(_gameWindow.topLeft.y < 0)
        _gameWindow.topLeft.y = 0;
    else if(_gameWindow.botRight.y > _levelBounds.botRight.y)
    {
        _gameWindow.botRight.y = _levelBounds.botRight.y;
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


CSRect* CSGameState::getGameWindow(void)
{
    return &_gameWindow;
}

CSRect* CSGameState::getLevelBounds(void)
{
    return &_levelBounds;
}

bool CSGameState::getRoomNumsState(void)
{
    return _printRoomNums;
}

bool CSGameState::getBreakState(void)
{
    return _breakForDebug;
}

CSCreature* CSGameState::getPlayer(void)
{
    return &_theplayer;
}

int CSGameState::getLevelMonsterManual(int inLevel, list<CSMonsterClass *> &inMonsterList)
{
    CSMonsterClass  *theMonster;
    list<CSMonsterClass *>::iterator    listIter;
    
    for(listIter = _monsterManual.begin(); listIter != _monsterManual.end(); listIter++)
        if((*listIter)->getAppearing(inLevel))
        {
            theMonster = *listIter;
            inMonsterList.push_back(theMonster);
        }
    
    return (int)inMonsterList.size();
}
