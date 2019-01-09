//
//  CSDoorHandler.hpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 11/18/18.
//  Copyright © 2018 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSDoorHandler_hpp
#define CSDoorHandler_hpp

#include <stdio.h>
#include <list>
#include "WanderFile.h"
#include "CSRandomHandler.hpp"
#include "CSRandomList.hpp"
#include "CSDungObj.hpp"

class CSDoorHandler
{
private:
    int                 _numDoors;
    list<CSDungObj *>   _unconnectedLevelDoors;
    CSRandomHandler     *_theRandHand;
    CSRandomList        _numNewDoors;
    
public:
    CSDoorHandler();
    void clear(void);
    
    void addDoor(CSDungObj *);
    CSDungObj* getNextDoor(void);
    void removeDoor(CSDungObj *);
    int getNewDoorQuantity(void);
    
    int getNumDoors(void);
    int getSize(void);
};

#endif /* CSDoorHandler_hpp */
