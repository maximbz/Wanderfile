//
//  CSLine.hpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 10/17/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSLine_hpp
#define CSLine_hpp

#include <stdio.h>

class CSLine
{
private:
    int _start, _end, _perpLoc;
    
public:
    
    CSLine();
    CSLine(int, int);
    CSLine(int, int, int);
    
    void setLine(int, int);
    void setLine(int, int, int);
    void setStart(int);
    void setEnd(int);
    void setPerpLoc(int);
    
    bool doesContain(int);
    bool isEndGreaterThanStart(void);
    int getSize(void);
    
    int getStart(void);
    int getEnd(void);
    int getPerpLoc(void);
};

#endif /* CSLine_hpp */
