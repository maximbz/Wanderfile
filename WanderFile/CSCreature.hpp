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
#include "CSMonsterClass.hpp"
#include "CSDungObj.hpp"

using namespace std;

class CSCreature
{
private:
    bool        _player;
    int         _hp, _atk, _ac, _xp;
    string      _name;
    CSDungObj   _creatureObj;
    
public:
    CSCreature();
    CSCreature(bool, CSPoint *);
    CSCreature(CSPoint *, CSMonsterClass *);
    
    void moveCreature(CSPoint *);
    
    void setIsPlayer(bool);
    void setLoc(CSPoint *);
    void changeHP(int);
    void killCreature(void);
    bool doesPlayerHit(int);
    int attackPlayer(void);
    
    bool getIsPlayer(void);
    CSPoint* getLoc(void);
    int getXP(void);
    string getName(void);
    
    CSDungObj* getCreatureObj(void);
};

#endif /* CSCreature_hpp */
