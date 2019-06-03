//
//  CSMonsterClass.cpp
//  WanderFile
//
//  Created by Maxim Boschert Zielsdorf on 5/26/19.
//  Copyright © 2019 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSMonsterClass.hpp"

CSMonsterClass::CSMonsterClass(char inChar, int inHP, int inAtk, int inAC, int inXP, string inName, CSRange *inAppearing)
{
    _char = inChar;
    _hp = inHP;
    _atk = inAtk;
    _ac = inAC;
    _xp = inXP;
    _name = inName;
    _appearing = *inAppearing;
}


char CSMonsterClass::getChar(void)
{
    return _char;
}

int CSMonsterClass::getHP(void)
{
    return _hp;
}

int CSMonsterClass::getAtk(void)
{
    return _atk;
}

int CSMonsterClass::getAC(void)
{
    return _ac;
}

int CSMonsterClass::getXP(void)
{
    return _xp;
}

string CSMonsterClass::getName(void)
{
    return _name;
}

bool CSMonsterClass::getAppearing(int inLevel)
{
    return _appearing.doesContain(inLevel);
}
