//
//  CSRandomRange.hpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/13/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSRandomRange_hpp
#define CSRandomRange_hpp

#include <stdio.h>
#include <string>
#include "CSRange.hpp"

using namespace std;

class CSRandomRange
{
private:
    string  _rangeName;
    CSRange _range;
    
public:
    CSRandomRange(string, int, int);
    CSRandomRange(string, CSRange);
    
    string getRangeName(void);
    CSRange getRange(void);
    int getRangeMin(void);
    int getRangeMax(void);
};

#endif /* CSRandomRange_hpp */
