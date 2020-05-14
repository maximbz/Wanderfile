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
#include <ncurses.h>
#include "CSGameState.hpp"
#include "WanderFile.h"
#include "CSRange.hpp"
#include "CSDungeonLevel.hpp"

using namespace std;

CSGameState::CSGameState(void)
{
    theDoorHand.addRandomHandler(&theRandHand);
    
    //set up menus
    _slideOptions.push_back('a');
    _slideOptions.push_back('d');
    _slideOptions.push_back('w');
    _slideOptions.push_back('s');
    
    _mainModeOptions.push_back('n');
    _mainModeOptions.push_back('l');
    _mainModeOptions.push_back('v');
    
    _gameOptions.push_back('b');
    _gameOptions.push_back('q');
    
    _menuSelection.addCharVect(&_slideOptions);
    _menuSelection.addCharVect(&_mainModeOptions);
    _menuSelection.addCharVect(&_gameOptions);
    
    _menuMessage = "";
    
    //set up gameplay
    _printRoomNums = true;//false
    _breakForDebug = false;
    _levelNum = 1;
    
    _levelBounds.setPoints(0, 0, LEVEL_BOUND_RIGHT, LEVEL_BOUND_BOTTOM);
    _gameWindRect.setPoints((LEVEL_BOUND_RIGHT / 2) - (WINDOW_BOUND_RIGHT / 2) + 1, (LEVEL_BOUND_BOTTOM / 2) - (WINDOW_BOUND_BOTTOM / 2) + 1,
                            (LEVEL_BOUND_RIGHT / 2) + (WINDOW_BOUND_RIGHT / 2), (LEVEL_BOUND_BOTTOM / 2) + (WINDOW_BOUND_BOTTOM / 2));
    
    //set up windows
    _gameWind = newwin(_gameWindRect.getHeight(), _gameWindRect.getWidth(), 1, 1);
    _gameWind = initscr();//initializes terminal to use ncurses
    _menuWind = newwin(MENU_BOUND_HEIGHT, _gameWindRect.getWidth(), _gameWindRect.getHeight() + 1, 0);
    _menuWind = initscr();//initializes terminal to use ncurses
    cbreak();//disable the buffering of typed characters by the TTY driver and get a character-at-a-time input
    noecho();//keeps typed keys from automatically echoing to terminal
    clear();
    refresh();
    curs_set(0);//set cursor to be invisible
    start_color();
    
    _playerMoveRect.setPoints(0, 0, PLAYER_MOVE_BOUND_RIGHT, PLAYER_MOVE_BOUND_BOTTOM);
    _thePlayer.setIsPlayer(true, this);
    if(loadMonsterManual())
        printf("Monsters did not load. Is the file 'MonsterManual.txt' missing?\n");
    
    _theDungeon.dungeonLevelInit(this);
    theBeHand.behaviorHandlerInit(this);
}


#pragma mark -
#pragma mark Doers - Init Functions

void CSGameState::setGameWindow(CSRect inRect)
{
    _gameWindRect = inRect;
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
    inputFile.open("./Developer/Wanderfile/MonsterManual.txt");
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
        stringstream strToInt(valueString);
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


#pragma mark -
#pragma mark Doers - Doers

void CSGameState::centerGameWindow(CSPoint *inPoint)
{
    bool    topLeftAnchor = true;
    
    //center on in point. If that puts gameWindow outside of LEVEL_BOUNDS, set gameWindow to LEVEL_BOUNDS
    
    _gameWindRect.topLeft.x =  inPoint->x - (WINDOW_BOUND_RIGHT / 2);
    if(_gameWindRect.topLeft.x < 0)
        _gameWindRect.topLeft.x = 0;
    else if(_gameWindRect.botRight.x > _levelBounds.botRight.x)
    {
        _gameWindRect.botRight.x = _levelBounds.botRight.x;
        topLeftAnchor = false;
    }
    
    _gameWindRect.topLeft.y = inPoint->y - (WINDOW_BOUND_BOTTOM / 2);
    if(_gameWindRect.topLeft.y < 0)
        _gameWindRect.topLeft.y = 0;
    else if(_gameWindRect.botRight.y > _levelBounds.botRight.y)
    {
        _gameWindRect.botRight.y = _levelBounds.botRight.y;
        topLeftAnchor = false;
    }
    
    
    //adjust other corner based on the corner we set
    
    if(topLeftAnchor)
    {
        _gameWindRect.botRight.x = _gameWindRect.topLeft.x + WINDOW_BOUND_RIGHT;
        _gameWindRect.botRight.y = _gameWindRect.topLeft.y + WINDOW_BOUND_BOTTOM;
    }
    else
    {
        _gameWindRect.topLeft.x = _gameWindRect.botRight.x - WINDOW_BOUND_RIGHT;
        _gameWindRect.topLeft.y = _gameWindRect.botRight.y - WINDOW_BOUND_BOTTOM;
    }
}

void CSGameState::centerPlayerMoveRect(CSPoint *inPoint)
{
    bool    topLeftAnchor = true;
    
    //center on in point. If that puts gameWindow outside of LEVEL_BOUNDS, set gameWindow to LEVEL_BOUNDS
    
    _playerMoveRect.topLeft.x =  inPoint->x - (PLAYER_MOVE_BOUND_RIGHT / 2);
    if(_playerMoveRect.topLeft.x < 0)
        _playerMoveRect.topLeft.x = 0;
    else if(_playerMoveRect.botRight.x > _levelBounds.botRight.x)
    {
        _playerMoveRect.botRight.x = _levelBounds.botRight.x;
        topLeftAnchor = false;
    }
    
    _playerMoveRect.topLeft.y = inPoint->y - (PLAYER_MOVE_BOUND_BOTTOM / 2);
    if(_playerMoveRect.topLeft.y < 0)
        _playerMoveRect.topLeft.y = 0;
    else if(_playerMoveRect.botRight.y > _levelBounds.botRight.y)
    {
        _playerMoveRect.botRight.y = _levelBounds.botRight.y;
        topLeftAnchor = false;
    }
    
    
    //adjust other corner based on the corner we set
    
    if(topLeftAnchor)
    {
        _playerMoveRect.botRight.x = _playerMoveRect.topLeft.x + PLAYER_MOVE_BOUND_RIGHT;
        _playerMoveRect.botRight.y = _playerMoveRect.topLeft.y + PLAYER_MOVE_BOUND_BOTTOM;
    }
    else
    {
        _playerMoveRect.topLeft.x = _playerMoveRect.botRight.x - PLAYER_MOVE_BOUND_RIGHT;
        _playerMoveRect.topLeft.y = _playerMoveRect.botRight.y - PLAYER_MOVE_BOUND_BOTTOM;
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


bool CSGameState::gameLoop(void)
{
    bool    gameLoop = true, playLoop = true;
    string  outputString;
    
    while(playLoop)
    {
        _theDungeon.printWindow();
        
        if(_menuMessage != "")
        {
            mvwaddstr(_menuWind, WINDOW_BOUND_BOTTOM + 2, 35, _menuMessage.c_str());
            _menuMessage = "";
        }
        else
        {
            mvwaddstr(_menuWind, WINDOW_BOUND_BOTTOM + 1, 0, outputString.c_str());
            mvwaddstr(_menuWind, WINDOW_BOUND_BOTTOM + 2, 0, "(L)oad dungeon, Create (N)ew dungeon, Sa(V)e current dungeon, Toggle line (B)reak, or (Q)uit?\n");
        }
        
        _menuSelection.toggleCharOption(1, true);//turn main mode on
        _menuSelection.getUserCharAnswer(_menuSelectMatrix, _menuWind);
        
        //directions
        if(_menuSelectMatrix.x == 0)
        {
            if(_menuSelectMatrix.y == 0)
                movePlayer(REG_WALL_LEFT);
            if(_menuSelectMatrix.y == 1)
                movePlayer(REG_WALL_RIGHT);
            if(_menuSelectMatrix.y == 2)
                movePlayer(REG_WALL_TOP);
            if(_menuSelectMatrix.y == 3)
                movePlayer(REG_WALL_BOT);
        }
        
        //main menu commands
        if(_menuSelectMatrix.x == 1)
        {
            if(_menuSelectMatrix.y == 0)
                _theDungeon.createDungeon(_levelNum);
            if(_menuSelectMatrix.y == 1)
                _theDungeon.loadDungeon(&outputString);
            if(_menuSelectMatrix.y == 2)
                _theDungeon.saveDungeon();
        }
        
        //game commends
        if(_menuSelectMatrix.x == 2)
        {
            if(_menuSelectMatrix.y == 0)//toggle line break
            {
                toggleBreak();
                
                if(_breakForDebug == true)
                    gameLoop = true;
            }
            if(_menuSelectMatrix.y == 1)//quit
            {
                gameLoop = false;
                playLoop = false;
            }
        }
    }
    
    _theDungeon.deleteDungeon();
    return gameLoop;
}

void CSGameState::slideGameWindow(entReg inReg)
{
    int     loop;
    CSPoint dimVect(0, 0), inVect(&dimVect, inReg);
    
    for(loop = AXIS_HORIZ; loop <= AXIS_VERT; loop++)
    {
        if((_gameWindRect.topLeft.getAxisPoint((axis)loop) > 1 && inVect.getAxisPoint((axis)loop) < 0) ||
           (_gameWindRect.botRight.getAxisPoint((axis)loop) < _levelBounds.botRight.getAxisPoint((axis)loop) && inVect.getAxisPoint((axis)loop) > 0))
        {
            dimVect.setAxisPoint((axis)loop, inVect.getAxisPoint((axis)loop));
            _gameWindRect.slideRect(&dimVect);
        }
        
        dimVect.setPoints(0, 0);
    }
    
    //printf("Game Window: %d, %d - %d, %d\n", _gameWindRect.topLeft.x, _gameWindRect.topLeft.y, _gameWindRect.botRight.x, _gameWindRect.botRight.y);
}

void CSGameState::slidePlayerMoveRect(entReg inReg)
{
    int     loop;
    CSPoint dimVect(0, 0), inVect(&dimVect, inReg);
    
    for(loop = AXIS_HORIZ; loop <= AXIS_VERT; loop++)
    {
        if((_playerMoveRect.topLeft.getAxisPoint((axis)loop) > 1 && inVect.getAxisPoint((axis)loop) < 0) ||
           (_playerMoveRect.botRight.getAxisPoint((axis)loop) < _levelBounds.botRight.getAxisPoint((axis)loop) && inVect.getAxisPoint((axis)loop) > 0))
        {
            dimVect.setAxisPoint((axis)loop, inVect.getAxisPoint((axis)loop));
            _playerMoveRect.slideRect(&dimVect);
        }
        
        dimVect.setPoints(0, 0);
    }
}

void CSGameState::movePlayer(entReg inReg)
{
    CSPoint     oldLoc = *_thePlayer.getLoc(), newLoc(_thePlayer.getLoc(), inReg);
    
    _thePlayer.moveCreature(inReg);//move the player
    
    if((!getPlayerMoveRect()->doesRectContainPoint(&newLoc)) && oldLoc != *_thePlayer.getLoc())
    {
        slidePlayerMoveRect(inReg);
        slideGameWindow(inReg);
    }
    
    _theDungeon.updateRooms(_thePlayer.getLoc());
}

void CSGameState::gainTreasure(void)
{
    _menuMessage = "Treasure Get!";
}


#pragma mark -
#pragma mark Doers - Getters

WINDOW* CSGameState::getGameWindow(void)
{
    return _gameWind;
}

CSRect* CSGameState::getPlayerMoveRect(void)
{
    return &_playerMoveRect;
}

CSRect* CSGameState::getGameWindRect(void)
{
    return &_gameWindRect;
}

CSRect* CSGameState::getLevelBounds(void)
{
    return &_levelBounds;
}

int CSGameState::getLevelNum(void)
{
    return _levelNum;
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
    return &_thePlayer;
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
