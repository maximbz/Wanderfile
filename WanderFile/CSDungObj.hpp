//
//  CSDungObj.hpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 11/20/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSDungObj_hpp
#define CSDungObj_hpp

#include <stdio.h>
#include <vector>
#include "CSPoint.hpp"

using namespace std;

class CSRoom;

class CSDungObj
{
private:
    bool        _wasMoved;
    char        _objectChar;
    objType     _objectType;
    objReg      _objectRegion;
    CSPoint     _objectLoc;
    CSDungObj   *_parentObj, *_childObj,//the parent of a chain of objects has a child connected to it: parent -> _connectTo -> child; parent -> _connectFrom -> child
                *_connect;//doors connect to and from one another; there's no chain of command
    CSRoom      *_owner;
    
    
public:
    CSDungObj();
    CSDungObj(char, objType, objReg, CSPoint, CSDungObj*, CSDungObj*, CSRoom*);
    
    void setWasMoved(bool);
    void setChar(char);
    void setType(objType);
    void setRegion(objReg);
    void setLoc(CSPoint);
    void setParent(CSDungObj*);
    void setChild(CSDungObj*);
    void setConnect(CSDungObj*);
    void setOwner(CSRoom*);
    
    void slideObject(CSPoint);
    bool slideDoor(CSPoint);
    bool checkForRegion(objReg);
    void removeConnect(void);
    void deleteObject(void);
    
    bool getWasMoved(void);
    char getChar(void);
    objType getType(void);
    objReg getRegion(void);
    CSPoint* getLoc(void);
    CSRoom* getOwner(void);
    CSDungObj* getParent(void);
    CSDungObj* getConnect(void);
    CSDungObj* getChild(void);
};

#endif /* CSDungObj_hpp */
