//
//  CSRect.hpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 1/28/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSRect_hpp
#define CSRect_hpp

#include <stdio.h>
#include "WanderFile.h"
#include "CSPoint.hpp"
#include "CSRange.hpp"

class CSRect
{
private:
    int _rectWidth, _rectHeight;
    
public:
    CSPoint topLeft, botRight;
    
    CSRect();
    CSRect(CSPoint *, CSPoint *);
    CSRect(int, int, int, int);
    
    void setPoints(CSPoint *, CSPoint *);
    void setPoints(int, int, int, int);
    void setTopLeft(CSPoint *);
    void setTopLeft(int, int);
    void setBotRight(CSPoint *);
    void setBotRight(int, int);
    
    //dynamic (Irish) setters
    bool setWallLoc(objReg, int);//set the point, or perpendicular axis, based on incomingWall
    void setWallRange(objReg, CSRange *);//start and end points based on incomingWall
    void setCorner(direction, CSPoint *);//topLeftor botRight
    
    void calculateArea(void);
    void slideRect(CSPoint *);
    bool doesRectContainPoint(CSPoint *);
    //bool doesWallContainPoint(objReg, CSPoint *);
    bool doesRectContainWall(CSRect *, objReg);
    
    int getWidth(void);
    int getHeight(void);
    int getDim(axis);//get heidth or width of room
    CSPoint getCenterPoint(void);
    
    //dynamic getters
    CSPoint* getCorner(direction);//topLeft or botRight from direction UP_LEFT or DOWN_RIGHT
    int getWallStartPoint(objReg);//top or left
    int getWallEndPoint(objReg);//bottom or right
    int getWallLocPoint(objReg);//x or y of entire wall
    CSRange getWallRange(objReg);//start and end based on incomingWall
    CSRange getAxisRange(axis);//start and end based on incomingAxis
    
    //operators?!
    CSRect& operator=(const CSRect&);
    CSRect operator+(const CSPoint&);
};

#endif /* CSRect_hpp */
