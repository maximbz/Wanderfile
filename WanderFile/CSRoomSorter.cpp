//
//  CSRoomSorter.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 11/22/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSRoomSorter.hpp"

CSRoomSorter::CSRoomSorter()
{
    
}

CSRoomSorter::CSRoomSorter(bool incomingSortType)
{
    _topDown = incomingSortType;
}

void CSRoomSorter::setSortMode(bool incomingMode)
{
    _topDown = incomingMode;
}

bool CSRoomSorter::operator()(CSRoom* &earlierItem, CSRoom* &laterItem)
{
    /*if((earlierItem->getRect()->topLeft.x < 0) || (earlierItem->getRect()->topLeft.y < 0) ||
       (earlierItem->getRect()->botRight.x > LEVEL_BOUND_RIGHT) || (earlierItem->getRect()->botRight.y > LEVEL_BOUND_BOTTOM))
        printf("Room %d is out of window bounds. This will result in unexpected behavior.\n", earlierItem->getRoomNum());*/
    
    if(_topDown)
        return earlierItem->getRect()->topLeft.y < laterItem->getRect()->topLeft.y;
    else
        return earlierItem->getRect()->topLeft.x < laterItem->getRect()->topLeft.x;
}
