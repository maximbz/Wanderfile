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
#include "CSLine.hpp"
#include "CSDungObj.hpp"

class CSDoorHandler
{
private:
    int                 _numDoors;
    list<CSLine>        _loadingObjConnectData;//Type (0 = Child, 1 = Connect), RoomNum, ObjNum
    list<CSDungObj *>   _unconnectedLevelDoors;
    CSRandomHandler     *_theRandHand;
    CSRandomList        _numNewDoorsRandList;
    CSRandomRange       _newDoorRandRange;
    
public:
    CSDoorHandler(CSRandomHandler *);
    void clear(void);
    
    void addDoor(CSDungObj *);
    void addLoadingObj(CSDungObj *, CSLine);
    CSDungObj * getNextDoor(void);
    CSLine getNextConnectData(void);
    void removeDoor(CSDungObj *);
    int getNewDoorQuantity(int);
    
    int getNumDoors(void);
    int getSize(void);
};

#endif /* CSDoorHandler_hpp */
