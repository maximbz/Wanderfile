//
//  CSBehaviorHandler.hpp
//  WanderFile
//
//  Created by Maxim Boschert Zielsdorf on 4/30/20.
//  Copyright © 2020 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSBehaviorHandler_hpp
#define CSBehaviorHandler_hpp

#include <stdio.h>
#include "WanderFile.h"

class CSGameState;
class CSEntity;


class CSBehaviorHandler
{
private:
    CSGameState *_theGame;
    
public:
    CSBehaviorHandler(void);
    void behaviorHandlerInit(CSGameState *);
    
    void adjacentBehavior(CSEntity *);
    void overlapBehavior(CSEntity *);
    
    char getChar(entType);
    bool isPassable(entType);
};



#endif /* CSBehaviorHandler_hpp */
