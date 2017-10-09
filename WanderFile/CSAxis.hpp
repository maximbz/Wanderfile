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
    PARALLEL,
    PERPENDICULAR
};

class CSAxis
{
public:
    axis        dim;
    direction   dir;
    
    
    CSAxis();
    CSAxis(axis, direction);
    
    void setAxis(axis, orientation);
    void setAxisFromWall(objReg);
    void setDir(direction, orientation);
    void setDirFromWall(objReg);
    
    axis getPerpAxis(void);
    int getAxisMod(int, orientation);
    axis getWallAxis(objReg);
    
    direction getWallDir(objReg);
    direction getOpposingDir(void);
    int getDirectionOffset(void);
    int getOpposingDirOffset(void);
};

#endif /* CSAxis_hpp */
