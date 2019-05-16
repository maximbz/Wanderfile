//
//  CSCreature.hpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 4/3/19.
//  Copyright © 2019 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSCreature_hpp
#define CSCreature_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "CSPoint.hpp"
#include "CSRange.hpp"
#include "CSDungObj.hpp"

using namespace std;

class CSCreature
{
private:
    bool        _player;
    int         _hp, _atk, _ac, _xp;
    string      _name;
    CSRange     _appearing;
    CSDungObj   _creatureObj;
    
public:
    CSCreature();
    CSCreature(bool, CSPoint *);
    
    void moveCreature(CSPoint *);
    
    void setIsPlayer(bool);
    void setLoc(CSPoint *);
    void setHP(int);
    void setAtk(int);
    void setAC(int);
    void setXP(int);
    void setName(string);
    void setAppearing(CSRange *);
    
    bool getIsPlayer(void);
    CSPoint* getLoc(void);
    int getHP(void);
    int getAtk(void);
    int getAC(void);
    int getXP(void);
    string getName(void);
    bool getAppearing(int);
    
    CSDungObj* getCreatureObj(void);
};

#endif /* CSCreature_hpp */
