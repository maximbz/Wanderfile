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
    char        _objectChar;
    int         _objectType;
    objReg      _objectRegion;
    CSPoint     _objectLoc;
    CSRoom      *_owner;
    CSDungObj   *_connectTo, *_connectFrom;
    
public:
    CSDungObj();
    CSDungObj(char, int, objReg, CSPoint, CSRoom*);
    CSDungObj(objReg, CSPoint, CSDungObj*, CSRoom*);
    
    void setChar(char);
    void setType(int);
    void setRegion(objReg);
    void setLoc(CSPoint);
    void setConnectTo(CSDungObj*);
    void setConnectFrom(CSDungObj*);
    void setOwner(CSRoom*);
    
    bool slideObject(CSPoint);
    void removeConnect(void);
    void deleteObject(void);
    
    char getChar(void);
    int getType(void);
    objReg getRegion(void);
    CSPoint* getLoc(void);
    CSDungObj* getConnect(void);
    CSRoom* getOwner(void);
};

#endif /* CSDungObj_hpp */
