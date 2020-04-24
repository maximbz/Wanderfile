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
#include <vector>
#include "CSPoint.hpp"
#include "CSRange.hpp"
#include "CSMonsterClass.hpp"
#include "CSEntity.hpp"
#include "CSRandomHandler.hpp"

using namespace std;

class CSCreature : public CSEntity
{
private:
    bool            _player;
    int             _hp, _atk, _ac, _xp;
    string          _name;
    vector<string>  _creatureDataKey;
    CSRandomHandler *_theRandHand;
    CSRandomRange   _moveDir;
    
    void initCreature(void);
public:
    CSCreature();
    CSCreature(CSPoint *, CSMonsterClass *, CSRoom *, CSRandomHandler *);
    CSCreature(CSRoom *, CSRandomHandler *, CSFileLoader *);//load entity from file
    
    void setIsPlayer(bool);
    
    bool moveCreature(entReg);
    void changeHP(int);
    void killCreature(void);
    bool doesPlayerHit(int);
    bool updateEntity(void);
    int attackPlayer(void);
    string printEntityToFile(void);
    
    bool getIsPlayer(void);
    int getXP(void);
    string getName(void);
};

#endif /* CSCreature_hpp */
