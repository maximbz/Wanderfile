//
//  CSRoomProx.hpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 6/24/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSRoomProx_hpp
#define CSRoomProx_hpp

#include <stdio.h>
#include "CSRoom.hpp"
#include "CSRange.hpp"

class CSRoomProx
{
private:
    CSRoom  *_nearbyRoom;
    CSRange _roomOverlap;
    int     _distance;
    
public:
    CSRoomProx();
    CSRoomProx(CSRoom*, CSRange, int);
    
    void setNearbyRoom(CSRoom*);
    void setRoomOverlap(CSRange);
    void setRoomOverlap(int, int);
    void setRoomOverlapMin(int);
    void setRoomOverlapMax(int);
    void setDistance(int);
    
    CSRoom* getNearbyRoom(void);
    CSRange getRoomOverlap(void);
    int getDistance(void);
};

#endif /* CSRoomProx_hpp */
