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
#include "CSRandomHandler.hpp"
#include "CSDoorHandler.hpp"
#include "CSGameState.hpp"
#include "CSRect.hpp"
#include "CSRange.hpp"
#include "CSRandomRange.hpp"
#include "CSDungObjSorter.hpp"

class CSDungObj;

class CSRoom
{
private:
    CSGameState         *_theGame;
    CSRandomHandler     *_theRandHand;
    CSDoorHandler       *_theDoorHand;
    
    bool                _isHall;
    int                 _roomNum, _roomNumDigits;
    CSRandomRange       _wallGenLoc[NUM_ROOM_WALLS];
    CSRect              _roomRect;
    list<CSDungObj*>    _objects;
    CSDungObjSorter     _objectComparator;
    CSRoom              *_roomToConnect;

    CSDungObj* checkForObject(CSPoint);
    
public:    
    CSRoom(CSGameState *, CSRandomHandler *, CSDoorHandler *);
    CSRoom(CSGameState *, CSRandomHandler *, CSDoorHandler *, CSPoint, CSPoint);
    
    void setHallState(bool);
    void setRoomNum(int);
    void setRoomToConnect(CSRoom *);
    
    CSDungObj* createObject(objType, objReg, CSPoint, CSDungObj *, CSDungObj *);
    void createNewDoor(objReg);
    void createNewObject(objType);
    void removeConnection(CSRoom *);
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
    
    char assumeChar(CSDungObj *, char);
    string printRoomRow(CSRange, int);
    string printRoomToFile(void);
    
    bool isHall(void);
    int getRoomNum(void);
    CSRandomRange* getWallGenRanges(void);
    list<CSDungObj *>* getObjects(void);
    CSRect* getRect(void);
    CSRect getWallessRect(void);
    CSRoom* getRoomToConnect(void);
};

#endif /* CSRoom_hpp */
