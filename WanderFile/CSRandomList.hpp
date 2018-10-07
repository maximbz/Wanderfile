//
//  CSRandomList.hpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 10/6/18.
//  Copyright © 2018 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSRandomList_hpp
#define CSRandomList_hpp

#include <stdio.h>
#include <vector>
#include "WanderFile.h"

using namespace std;

class CSRandomList
{
private:
    randType    _randType;
    vector<int> _numList;
    
public:
    CSRandomList();
    CSRandomList(randType);
    CSRandomList(randType, vector<int>*);
    
    void setRandType(randType);
    void setList(vector<int>*);
    
    void addNumToList(int);
    void addListToList(vector<int>*);
    
    randType getRandType(void);
    vector<int>* getList(void);
};

#endif /* CSRandomList_hpp */
