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
#include "CSPoint.hpp"
#include "CSRange.hpp"

class CSRect
{
private:
    int _rectWidth, _rectHeight;
    
public:
    CSPoint topLeft, botRight;
    
    CSRect();
    CSRect(CSPoint, CSPoint);
    CSRect(int, int, int, int);
    
    void setPoints(CSPoint, CSPoint);
    void setPoints(int, int, int, int);
    void setTopLeft(CSPoint);
    void setTopLeft(int, int);
    void setBotRight(CSPoint);
    void setBotRight(int, int);
    
    //dynamic (Irish) setters
    void setWallLoc(int, int);//set the point, or perpendicular axis, based on incomingWall
    void setWallRange(int, CSRange);//start and end points based on incomingWall
    void setCorner(int, CSPoint);//topLeftor botRight
    
    void calculateArea(void);
    void slideRect(CSPoint);
    
    int getWidth(void);
    int getHeight(void);
    int getDim(axis);//get heidth or width of room
    
    //dynamic getters
    CSPoint* getCorner(int);//topLeft or botRight from direction UP_LEFT or DOWN_RIGHT
    int getWallStartPoint(int);//top or left
    int getWallEndPoint(int);//bottom or right
    int getWallLocPoint(int);//x or y of entire wall
    CSRange getWallRange(int);//start and end based on incomingWall
    
    //operators?!
    CSRect& operator=(const CSRect&);
    CSRect operator+(const CSPoint&);
};

#endif /* CSRect_hpp */
