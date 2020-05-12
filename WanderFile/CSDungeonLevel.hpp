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
#include "CSRandomRange.hpp"
#include "CSRoom.hpp"
#include "CSRoomSorter.hpp"

class CSGameState;


class CSDungeonLevel
{
private:
    CSGameState        *_theGame;
    
    int                 _maxNumDoors;
    string              _fileName;
    CSPoint             _startingStairs;
    CSRect              _dungeonBounds;
    CSRoom              *_outerRooms[NUM_ROOM_WALLS];
    list<CSRoom *>      _levelRooms;
    CSRoomSorter        _roomComparator;
    //list<CSCreature *>  _levelMonsters;
    
    //void indexRooms(list<CSRoom*>*);
    CSRoom* createFirstRoom(void);
    bool createRoomGenRanges(CSEntity *, CSRoom *);
    bool createNewRoom(CSEntity *, CSRoom *, int*);
    void createOuterDoor(void);
    void createStairs(void);
    void createTreasure(void);
    void createMonsters(void);
    
public:
    CSDungeonLevel(void);
    void dungeonLevelInit(CSGameState *);
    
    void createDungeon(int);
    int saveDungeon(void);
    int loadDungeon(string *);
    void deleteDungeon(void);
    
    void updateDungeonBounds(CSRoom *);
    void updateRoomNums(void);
    void abortRoomGenPath(CSRoom *);
    void deleteRoom(CSRoom *);
    void replaceDoor(void);
    void slideRoom(int, int, int);
    void addRoomsToList(list<CSRoom *> *, CSRoom *, int);
    
    void updateRooms(CSPoint *);
    
    void printWindow();
    
    CSRoom* getRoomFromTile(CSPoint *);
    int getNumRooms(void);
};

#endif /* CSDungeon_hpp */
