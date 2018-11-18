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
#include "WanderFile.h"
#include "CSGameState.hpp"
#include "CSRect.hpp"
#include "CSRange.hpp"
#include "CSRandomRange.hpp"
#include "CSRandomHandler.hpp"
#include "CSDungObjSorter.hpp"

class CSDungObj;

class CSRoom
{
private:
    CSGameState         *_theGame;
    CSRandomHandler     *_theRandHand;
    
    bool                _isHall;
    int                 _roomNum, _roomNumDigits, _roomToConnectDist;
    CSRandomRange       _wallGenLoc[NUM_ROOM_WALLS];
    CSRect              _roomRect;
    list<CSDungObj*>    _objects;
    CSDungObjSorter     _objectComparator;
    CSRoom              *_roomToConnect;

    char checkForObject(CSPoint, char);//input assumes a given tile, returns possible replacement object instead, or the same assumed tile.
    
public:    
    CSRoom(CSGameState *, CSRandomHandler *);
    CSRoom(CSGameState *, CSRandomHandler *, CSPoint, CSPoint);
    
    void setHallState(bool);
    void setRoomNum(int);
    void setRoomToConnect(CSRoom *);
    void setRoomToConnectDist(int);
    
    CSDungObj* createObject(objType, objReg, CSPoint, CSDungObj*, CSDungObj*);
    void createNewDoor(void);
    void removeConnection(CSRoom*);
    void deleteRoom(void);
    void deleteObject(int);
    void deleteObject(CSDungObj *);

    int connectToRoom(void);
    void updateRoomNum(int);
    void updateObjectNums(void);
    CSDungObj* getUnconnectedDoor(void);
    CSDungObj* getConnectedDoor(void);//only for hallways
    CSDungObj* getDoorConnectedToRoom(CSRoom *);
    bool isWallPointFree(CSPoint, objReg, CSDungObj *);
    bool slideRoom(CSPoint);
    bool slideWall(objReg, int);
    string printRoomRow(CSRange, int);
    string printRoomToFile(void);
    
    bool isHall(void);
    int getRoomNum(void);
    CSRandomRange* getWallGenRanges(void);
    list<CSDungObj*>* getObjects(void);
    CSRect* getRect(void);
    CSRoom* getRoomToConnect(void);
    int getRoomToConnectDist(void);
};

#endif /* CSRoom_hpp */
