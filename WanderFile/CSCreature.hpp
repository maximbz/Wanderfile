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
#include "CSMonsterClass.hpp"
#include "CSPoint.hpp"
#include "CSRange.hpp"
#include "CSRandomRange.hpp"
#include "CSEntity.hpp"


using namespace std;

class CSCreature : public CSEntity
{
private:
    bool            _player;
    int             _hp, _atk, _ac, _xp;
    string          _name;
    vector<string>  _creatureDataKey;
    CSRandomRange   _moveDir;
    
    void initCreature(void);
public:
    CSCreature(void);
    CSCreature(CSGameState *, CSPoint *, CSMonsterClass *, CSRoom *);
    CSCreature(CSGameState *, CSRoom *, CSFileLoader *);//load entity from file
    
    void setIsPlayer(bool, CSGameState *);
    
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
