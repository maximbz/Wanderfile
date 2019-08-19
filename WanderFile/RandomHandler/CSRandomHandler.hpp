//
//  CSRandomHandler.hpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/1/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSRandomHandler_hpp
#define CSRandomHandler_hpp

#include <random>
#include "WanderFile.h"
#include "CSRandomRange.hpp"
#include "CSRandomList.hpp"

using namespace std;

class CSRandomHandler
{
private:
    mt19937                 _randomGenerator;
    vector<CSRandomRange>   _ranges;
    vector<CSRandomList>    _lists;
public:
    CSRandomHandler(void);
    
    void addRandomRange(CSRandomRange);
    void addRandomList(CSRandomList);
    void clearRandomItems(randType);
    
    int getNumber(CSRandomRange*);
    int getNumber(CSRandomList*);
};

#endif /* CSRandomHandler_hpp */
