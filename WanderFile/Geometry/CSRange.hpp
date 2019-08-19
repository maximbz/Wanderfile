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
private:
    int _min, _max;
    
public:
    
    CSRange();
    CSRange(int, int);
    
    void setRange(int, int);
    void setMin(int);
    void setMax(int);
    
    bool doesContain(int);
    
    int getMin(void);
    int getMax(void);
    int getSize(void);
};

#endif /* CSRange_hpp */
