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
#include "CSRandomHandler.hpp"

using namespace std;

class CSCreature : public CSDungObj
{
private:
    bool            _player;
    int             _hp, _atk, _ac, _xp;
    string          _name;
    CSRandomHandler *_theRandHand;
    CSRandomRange   _moveDir;
    
public:
    CSCreature();
    //CSCreature(bool, CSPoint *);
    CSCreature(CSPoint *, CSMonsterClass *, CSRoom *, CSRandomHandler *);
    void initCreature(void);
    
    bool moveCreature(objReg);
    
    void setIsPlayer(bool);
    void setNullOwner(void);
    
    void changeHP(int);
    void killCreature(void);
    bool doesPlayerHit(int);
    bool updateObject(void);
    int attackPlayer(void);
    
    bool getIsPlayer(void);
    int getXP(void);
    string getName(void);
};

#endif /* CSCreature_hpp */
