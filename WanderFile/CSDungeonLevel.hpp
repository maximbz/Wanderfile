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
    CSRandomHandler     *_theRandHand;
    CSDoorHandler       *_theDoorHand;
    CSGameState         *_theGame;
    
    int                 _levelNum, _maxNumDoors;
    string              _fileName;
    CSPoint             _startingStairs;
    CSRect              _dungeonBounds;
    CSRoom              *_outerRooms[NUM_ROOM_WALLS];
    list<CSRoom *>      _levelRooms;
    CSRoomSorter        _roomComparator;
    //list<CSCreature *>  _levelMonsters;
    
    //void indexRooms(list<CSRoom*>*);
    CSRoom* createFirstRoom(void);
    bool createRoomGenRanges(CSDungObj *, CSRoom *);
    bool createNewRoom(CSDungObj *, CSRoom *, int*);
    void createOuterDoor(void);
    void createStairs(void);
    void createTreasure(void);
    void createMonsters(void);
    
public:
    CSDungeonLevel(CSRandomHandler *, CSGameState *, CSDoorHandler *, int);
    
    void createDungeon(void);
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
    
    //bool checkForMonsterAtPoint(CSPoint *, CSDungObj *);
    void movePlayer(objReg);
    
    void printWindow();
    
    CSRoom* getRoomFromTile(CSPoint *);
    int getLevelNumber(void);
    int getNumRooms(void);
};

#endif /* CSDungeon_hpp */
