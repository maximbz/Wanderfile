//
//  CSPoint.hpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 11/25/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSPoint_hpp
#define CSPoint_hpp

#include <stdio.h>
#include "WanderFile.h"

class CSPoint
{
public:
    int x, y;//making these public just makes life way easier
    
    CSPoint();
    CSPoint(int, int);
    
    void setAxisPoint(int, int);//dynamic input of x or y
    void setPoints(int, int);
    
    void slidePoint(int, int);
    void slidePoint(CSPoint);
    
    int getAxisPoint(int);//dynamic return of x value or y value from axis (horiz or vert) input
    
    bool operator==(const CSPoint&);
    bool operator!=(const CSPoint&);
    bool operator>(const CSPoint&);
    bool operator>=(const CSPoint&);
    bool operator<(const CSPoint&);
    bool operator<=(const CSPoint&);
    
    CSPoint operator*(int);
    CSPoint operator+(CSPoint&);
};

#endif /* CSPoint_hpp */
