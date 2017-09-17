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
#include "CSRect.hpp"

class CSGameState
{
private:
    CSRect  _gameWindow;
    bool    _printRoomNums, _breakForDebug;
    
public:
    CSGameState();
    
    void setGameWindow(CSRect);
    
    void slideGameWindow(int, int);
    void centerGameWindow(CSPoint);
    void toggleRoomNums(void);
    void toggleBreak(void);
    
    CSRect getGameWindow(void);
    bool getRoomNumsState(void);
    bool getBreakState(void);
};

#endif /* CSGameState_hpp */
