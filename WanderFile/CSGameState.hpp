//
//  CSGameState.hpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 1/28/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSGameState_hpp
#define CSGameState_hpp

#include <stdio.h>
#include <list>
#include <ncurses.h>
#include "CSRect.hpp"
#include "CSRandomHandler.hpp"
#include "CSDoorHandler.hpp"
#include "CSBehaviorHandler.hpp"
#include "CSPlayerChoice.hpp"
#include "CSMonsterClass.hpp"
#include "CSDungeonLevel.hpp"
#include "CSCreature.hpp"


class CSGameState
{
private:
    bool            _printRoomNums, _breakForDebug;
    int             _levelNum;
    WINDOW          *_menuWind, *_gameWind;
    string          _menuMessage;
    CSRect          _playerMoveRect, _gameWindRect, _levelBounds;
    vector<char>    _gameOptions, _slideOptions, _mainModeOptions;
    CSPoint         _menuSelectMatrix;
    CSPlayerChoice  _menuSelection;
    CSCreature      _thePlayer;
    CSDungeonLevel  _theDungeon;
    
    list<CSMonsterClass *>   _monsterManual;
    
public:
    //public handlers are quicker to access
    CSRandomHandler      theRandHand;
    CSDoorHandler        theDoorHand;
    CSBehaviorHandler    theBeHand;
    
    CSGameState(void);
    
    void setGameWindow(CSRect);
    int loadMonsterManual(void);
    void cleanUpGameState(void);
    
    void centerGameWindow(CSPoint *);
    void centerPlayerMoveRect(CSPoint *);
    void toggleRoomNums(void);
    void toggleBreak(void);
    
    bool gameLoop(void);
    void slideGameWindow(entReg);
    void slidePlayerMoveRect(entReg);
    void movePlayer(entReg);
    void gainTreasure(void);
    
    WINDOW* getGameWindow(void);
    CSRect* getPlayerMoveRect(void);
    CSRect* getGameWindRect(void);
    CSRect* getLevelBounds(void);
    int getLevelNum(void);
    bool getRoomNumsState(void);
    bool getBreakState(void);
    CSCreature* getPlayer(void);
    int getLevelMonsterManual(int, list<CSMonsterClass *> &);
};

#endif /* CSGameState_hpp */
