//
//  CSRange.hpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 6/15/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSRange_hpp
#define CSRange_hpp

#include <stdio.h>

class CSRange
{
public:
    int min, max;
    
    CSRange();
    CSRange(int, int);
    
    void setRange(int, int);
    
    bool doesContain(int);
    
    int getSize();
};

#endif /* CSRange_hpp */
