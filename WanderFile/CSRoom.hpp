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
#include "CSRandomHandler.hpp"
#include "CSDungObjSorter.hpp"

class CSDungObj;

class CSRoom
{
private:
    CSGameState         *_theGame;
    
    bool                _isHall;
    int                 _roomNum, _roomNumDigits;
    CSRect              _roomRect;
    list<CSDungObj*>    _objects;
    CSDungObjSorter     _objectComparator;

    char checkForObject(CSPoint, char);//input assumes a given "tile", returns possible replacement object instead, or the same assumed "tile".
    
public:    
    CSRoom(CSGameState *);
    CSRoom(CSGameState *, CSPoint, CSPoint);
    
    //void setPoints(int, int, int, int);
    void setHallState(bool);
    void setRoomNum(int);
    
    CSDungObj* createObject(char, objType, objReg, CSPoint, CSDungObj*, CSDungObj*);
    void addDoor(CSDungObj*, objReg, CSPoint, CSDungObj*, CSDungObj*, CSRoom*);
    void removeConnection(CSRoom*);
    void deleteRoom(void);

    void updateRoomNum(int);
    bool doesRoomAlign(axis, CSRoom *);
    bool isPointInFreeWall(CSPoint, objReg);
    bool slideRoom(CSPoint);
    bool slideWall(objReg, int);
    string printRoomRow(CSRange, int);
    string printRoomToFile(void);
    
    bool isHall(void);
    int getRoomNum(void);
    list<CSDungObj*>* getObjects(void);
    CSRect* getRect(void);
};

#endif /* CSRoom_hpp */
