//
//  CSRoom.hpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/2/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSRoom_hpp
#define CSRoom_hpp

#include <list>
#include <vector>
#include "WanderFile.h"
#include "CSRandomHandler.hpp"
#include "CSDoorHandler.hpp"
#include "CSFileLoader.hpp"
#include "CSGameState.hpp"
#include "CSRect.hpp"
#include "CSRange.hpp"
#include "CSRandomRange.hpp"
#include "CSEntitySorter.hpp"

class CSEntity;

class CSRoom
{
private:
    CSGameState         *_theGame;
    CSRandomHandler     *_theRandHand;
    CSDoorHandler       *_theDoorHand;
    
    bool                _isHall, _vertHall;
    int                 _roomNum, _roomNumDigits, _numDoors;
    CSRandomRange       _wallGenLoc[NUM_ROOM_WALLS];
    CSRect              _roomRect;
    list<CSEntity*>     _entities;
    vector<string>      _roomDataKey;
    CSEntitySorter      _entComparitor;
    CSRoom              *_roomToConnect;
    
    void roomInit(CSGameState *, CSRandomHandler *, CSDoorHandler *);
    char assumeChar(CSEntity *, char);
    CSEntity* createEntity(entType, entReg, CSPoint *, CSEntity *, CSEntity *);
    CSEntity* checkForEntityToDraw(CSPoint *);
    
public:    
    CSRoom(CSGameState *, CSRandomHandler *, CSDoorHandler *);
    CSRoom(CSGameState *, CSRandomHandler *, CSDoorHandler *, CSPoint *, CSPoint *);
    CSRoom(CSGameState *, CSRandomHandler *, CSDoorHandler *, CSFileLoader *);//load room from file
    
    void setHallState(bool);
    void setVertHallState(bool);
    void setRoomNum(int);
    void setRoomToConnect(CSRoom *);
    
    void createCoreDoor(entReg, CSPoint *, CSEntity*);
    void createNewDoor(entReg);
    CSEntity* createNewEntity(entType);
    void addEntity(CSEntity *);
    void removeEntity(CSEntity *);
    void removeConnection(CSRoom *);
    void deleteRoom(void);
    void deleteEntity(int);
    void deleteEntity(CSEntity *);

    int connectToRoom(void);
    void updateRoomNum(int);
    void updateEntityNums(void);
    bool slideRoom(CSPoint *);
    bool slideWall(entReg, int);
    
    string printRoomRow(CSRange *, int);
    string printRoomToFile(void);
    
    CSEntity* getUnconnectedDoor(void);
    CSEntity* getConnectedDoor(void);//only for hallways
    CSEntity* getDoorConnectedToRoom(CSRoom *);
    CSEntity* getEntityAtTile(CSPoint *);
    bool getGoodRoomPoint(CSPoint &, bool);
    bool isWallPointFree(CSPoint *, entReg, CSEntity *);
    bool isTilePassable(CSPoint *);
    
    bool isHall(void);
    bool isVertHall(void);
    int getRoomNum(void);
    CSRandomRange* getWallGenRanges(void);
    list<CSEntity *>* getEntities(void);
    CSEntity * getEntityWithNum(int);
    CSRect* getRect(void);
    void getWallessRect(CSRect &);
    CSRoom* getRoomToConnect(void);
    int getNumDoors(void);
    CSGameState* getTheGame(void);
};

#endif /* CSRoom_hpp */
