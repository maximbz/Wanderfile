//
//  CSAxis.hpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 9/16/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSAxis_hpp
#define CSAxis_hpp

#include <stdio.h>
#include "WanderFile.h"

enum axis//axes/dimensions
{
    AXIS_NULL = -1,
    AXIS_HORIZ,
    AXIS_VERT
};

enum direction//directions
{
    DIR_NULL = -1,
    DIR_UP_LEFT,
    DIR_DOWN_RIGHT
};

enum orientation
{
    PARA,//parallel
    PERP//perpendicular
};

class CSAxis
{
public:
    axis        dim;
    direction   dir;
    
    
    CSAxis();
    CSAxis(axis, direction);
    
    void setAxis(axis, orientation);
    void setAxisFromWall(entReg);//sets Dir as well
    void setDir(direction, orientation);
    
    axis getPerpAxis(void);
    int getAxisMod(int, orientation);
    entReg getReg(void);
    entReg getPerpReg(void);
    axis getWallAxis(entReg);
    direction getWallDir(entReg);
    
    direction getOppDir(void);
    int getDirOffset(void);
    int getOppDirOffset(void);
};

#endif /* CSAxis_hpp */
