//
//  CSDungeonLevel.hpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/3/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSDungeonLevel_hpp
#define CSDungeonLevel_hpp

#include <list>
#include "WanderFile.h"
#include "CSGameState.hpp"
#include "CSRandomHandler.hpp"
#include "CSRandomRange.hpp"
#include "CSRoom.hpp"
#include "CSRoomSorter.hpp"
#include "CSRoomProx.hpp"
#include "CSDungObj.hpp"
#include "CSAxis.hpp"

struct wallOverlap
{
    objReg  wall;
    int     distance;
};

class CSDungeonLevel
{
private:
    CSRandomHandler *_theRandHand;
    CSGameState     *_theGame;
    
    CSRoomSorter    _roomComparator;
    int             _levelNum;
    CSRect          _levelBounds;
    string          _fileName;
    list<CSRoom *>  _levelRooms;
    
    void indexRooms(list<CSRoom*>*);
    CSRoom* createRoom(CSRoom*);
    bool doesRoomOverlap(CSRoom*, wallOverlap[NUM_ROOM_WALLS]);
    vector<CSRoomProx> getNearbyRooms(CSRoom *, objReg);
    
public:
    CSDungeonLevel(CSRandomHandler *, CSGameState *, int);
    
    void createDungeon(void);
    int saveDungeon(void);
    int loadDungeon(void);
    void deleteDungeon(void);
    void updateLevelBounds(CSRoom *);
    void updateRoomNums(void);
    void deleteLastRoom(void);
    void slideRoom(int, int, int);
    void printWindow();
    
    int getLevelNumber(void);
    int getNumRooms(void);
};

#endif /* CSDungeon_hpp */
