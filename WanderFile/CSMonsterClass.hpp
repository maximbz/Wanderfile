//
//  CSMonsterClass.hpp
//  WanderFile
//
//  Created by Maxim Boschert Zielsdorf on 5/26/19.
//  Copyright © 2019 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSMonsterClass_hpp
#define CSMonsterClass_hpp

#include <stdio.h>
#include <string>
#include "CSRange.hpp"

using namespace std;

class CSMonsterClass
{
private:
    char        _char;
    int         _hp, _atk, _ac, _xp;
    string      _name;
    CSRange     _appearing;
    
public:
    CSMonsterClass(char, int, int, int, int, string, CSRange *);
    
    char getChar(void);
    int getHP(void);
    int getAtk(void);
    int getAC(void);
    int getXP(void);
    string getName(void);
    bool getAppearing(int);
};

#endif /* CSMonsterClass_hpp */
