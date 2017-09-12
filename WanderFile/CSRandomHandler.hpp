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
#include "CSRandomRange.hpp"

using namespace std;

class CSRandomHandler
{
private:
    mt19937                 _randomGenerator;
    vector<CSRandomRange>   _ranges;
public:
    CSRandomHandler(void);
    
    void addRandomRange(CSRandomRange);
    int getNumber(string);
};

#endif /* CSRandomHandler_hpp */
