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
#include "CSMonsterClass.hpp"
#include "CSCreature.hpp"

class CSGameState
{
private:
    bool        _printRoomNums, _breakForDebug;
    CSRect      _playerMoveRect, _gameWindRect, _levelBounds;
    CSCreature  _theplayer;
    WINDOW      *_gameWind;
    
    list<CSMonsterClass *>   _monsterManual;
    
public:
    CSGameState();
    
    void setGameWindow(CSRect);
    int loadMonsterManual(void);
    void cleanUpGameState(void);
    
    void slideGameWindow(entReg);
    void centerGameWindow(CSPoint *);
    void slidePlayerMoveRect(entReg);
    void centerPlayerMoveRect(CSPoint *);
    void toggleRoomNums(void);
    void toggleBreak(void);
    
    WINDOW* getGameWindow(void);
    CSRect* getPlayerMoveRect(void);
    CSRect* getGameWindRect(void);
    CSRect* getLevelBounds(void);
    bool getRoomNumsState(void);
    bool getBreakState(void);
    CSCreature* getPlayer(void);
    int getLevelMonsterManual(int, list<CSMonsterClass *> &);
};

#endif /* CSGameState_hpp */
