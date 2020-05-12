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
#include "CSEntity.hpp"


class CSDoorHandler
{
private:
    int                 _numDoors;
    list<CSEntity *>   _unconnectedLevelDoors;
    CSRandomHandler     *_theRandHand;
    CSRandomList        _numNewDoorsRandList;
    CSRandomRange       _newDoorRandRange;
    
public:
    CSDoorHandler(void);
    
    void addRandomHandler(CSRandomHandler *);
    void clear(void);
    void addDoor(CSEntity *);
    void addLoadingEnt(CSEntity *);
    CSEntity* getNextDoor(void);
    void removeDoor(CSEntity *);
    int getNewDoorQuantity(int);
    
    int getNumDoors(void);
    int getSize(void);
};

#endif /* CSDoorHandler_hpp */
