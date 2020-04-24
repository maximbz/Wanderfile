//
//  CSEntity.hpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 11/20/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSEntity_hpp
#define CSEntity_hpp

#include <stdio.h>
#include <vector>
#include <list>
#include "CSFileLoader.hpp"
#include "CSPoint.hpp"
#include "CSLine.hpp"

using namespace std;

class CSDoorHandler;
class CSRoom;

class CSEntity
{
protected:
    bool            _wasMoved;
    char            _entChar;//only used to override default (or with room numbers)
    int             _entNum;
    entType         _entityType;
    entReg          _entityRegion;
    vector<string>  _entDataKey;
    CSPoint         _entLoc, _loadConnect, _loadChild;//_load points tell roomNum and entNum of connects and children to a loading dungeonLevel
    CSEntity        *_parentEnt, *_childEnt,//the parent of a chain of entities has a child connected to it: parent -> _connectTo -> child; parent -> _connectFrom ->                   child
                    *_connect;//doors connect to and from one another; there's no chain of command
    CSRoom          *_owner;
    
    void dataKeysInit(void);
    
public:
    CSEntity();
    CSEntity(entType, entReg, CSPoint *, CSEntity *, CSEntity *, CSRoom *);
    CSEntity(CSRoom *, CSDoorHandler *, CSFileLoader *);//load entity from file
    
    void setWasMoved(bool);
    void setChar(char);
    void setNum(int);
    void setLoc(CSPoint *);
    virtual void setOwner(CSRoom *);
    void setParent(CSEntity *);
    void setChild(CSEntity *);
    void setConnect(CSEntity *);
    
    void slideEntity(CSPoint);
    bool slideDoor(CSPoint);
    bool checkForRegion(entReg);
    void removeConnect(void);
    void deleteEntity(void);
    virtual bool updateEntity(void);
    virtual string printEntityToFile(void);
    
    bool getWasMoved(void);
    char getChar(void);
    int getNum(void);
    entType getType(void);
    bool isPassable(void);
    entReg getRegion(void);
    CSPoint* getLoc(void);
    CSPoint* getLoadConnect(void);
    CSPoint* getLoadChild(void);
    CSRoom* getOwner(void);
    CSEntity* getParent(void);
    CSEntity* getConnect(void);
    CSEntity* getChild(void);
    virtual bool getIsPlayer(void);
};

#endif /* CSEntity_hpp */
