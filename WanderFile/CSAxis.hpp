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
#include "Wanderfile.h"

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
    void setAxisFromWall(objReg);//sets Dir as well
    void setDir(direction, orientation);
    
    axis getPerpAxis(void);
    int getAxisMod(int, orientation);
    objReg getReg(void);
    objReg getPerpReg(void);
    axis getWallAxis(objReg);
    direction getWallDir(objReg);
    
    direction getOppDir(void);
    int getDirOffset(void);
    int getOppDirOffset(void);
};

#endif /* CSAxis_hpp */
