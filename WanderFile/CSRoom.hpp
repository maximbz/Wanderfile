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
    
    bool                _isHall, _vertHall;
    int                 _roomNum, _roomNumDigits, _numDoors;
    CSRandomRange       _wallGenLoc[NUM_ROOM_WALLS];
    CSRect              _roomRect;
    list<CSDungObj*>    _objects;
    CSDungObjSorter     _objectComparator;
    CSRoom              *_roomToConnect;

    CSDungObj* createObject(objType, objReg, CSPoint *, CSDungObj *, CSDungObj *);
    CSDungObj* checkForObjectToDraw(CSPoint *);
    char assumeChar(CSDungObj *, char);
    
public:    
    CSRoom(CSGameState *, CSRandomHandler *, CSDoorHandler *);
    CSRoom(CSGameState *, CSRandomHandler *, CSDoorHandler *, CSPoint *, CSPoint *);
    CSRoom(CSGameState *, CSRandomHandler *, CSDoorHandler *, list<string> &);
    void roomInit(CSGameState *, CSRandomHandler *, CSDoorHandler *);
    
    void setHallState(bool);
    void setVertHallState(bool);
    void setRoomNum(int);
    void setRoomToConnect(CSRoom *);
    
    void createCoreDoor(objReg, CSPoint *, CSDungObj*);
    void createNewDoor(objReg);
    CSDungObj* createNewObject(objType);
    void addObject(CSDungObj *);
    void removeObject(CSDungObj *);
    void removeConnection(CSRoom *);
    void deleteRoom(void);
    void deleteObject(int);
    void deleteObject(CSDungObj *);

    int connectToRoom(void);
    void updateRoomNum(int);
    void updateObjectNums(void);
    bool slideRoom(CSPoint *);
    bool slideWall(objReg, int);
    
    string printRoomRow(CSRange *, int);
    string printRoomToFile(void);
    
    CSDungObj* getUnconnectedDoor(void);
    CSDungObj* getConnectedDoor(void);//only for hallways
    CSDungObj* getDoorConnectedToRoom(CSRoom *);
    CSDungObj* getObjectAtTile(CSPoint *);
    bool getGoodRoomPoint(CSPoint &, bool);
    bool isWallPointFree(CSPoint *, objReg, CSDungObj *);
    bool isTilePassable(CSPoint *);
    
    bool isHall(void);
    bool isVertHall(void);
    int getRoomNum(void);
    CSRandomRange* getWallGenRanges(void);
    list<CSDungObj *>* getObjects(void);
    CSDungObj * getObjectWithNum(int);
    CSRect* getRect(void);
    void getWallessRect(CSRect &);
    CSRoom* getRoomToConnect(void);
    int getNumDoors(void);
    CSGameState* getTheGame(void);
};

#endif /* CSRoom_hpp */
