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
#include "CSDoorHandler.hpp"
#include "CSRandomRange.hpp"
#include "CSRoom.hpp"
#include "CSRoomSorter.hpp"


class CSDungeonLevel
{
private:
    CSRandomHandler *_theRandHand;
    CSDoorHandler   *_theDoorHand;
    CSGameState     *_theGame;
    
    CSRoomSorter    _roomComparator;
    int             _levelNum, _maxNumDoors;
    CSRect          _levelBounds;
    string          _fileName;
    CSRoom          *_outerRooms[NUM_ROOM_WALLS];
    list<CSRoom *>  _levelRooms;
    
    //void indexRooms(list<CSRoom*>*);
    CSRoom* createFirstRoom(void);
    bool createRoomGenRanges(CSDungObj *, CSRoom *);
    bool createNewRoom(CSDungObj *, CSRoom *);
    
public:
    CSDungeonLevel(CSRandomHandler *, CSGameState *, CSDoorHandler *, int);
    
    void createDungeon(void);
    int saveDungeon(void);
    int loadDungeon(void);
    void deleteDungeon(void);
    void updateLevelBounds(CSRoom *);
    void updateRoomNums(void);
    void abortRoomGenPath(CSRoom *);
    void deleteRoom(CSRoom *);
    void replaceDoor(void);
    void slideRoom(int, int, int);
    void printWindow();
    
    int getLevelNumber(void);
    int getNumRooms(void);
};

#endif /* CSDungeon_hpp */
