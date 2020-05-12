//
//  CSRoomSorter.hpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 11/22/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSRoomSorter_hpp
#define CSRoomSorter_hpp

#include <stdio.h>
#include "WanderFile.h"


class CSRoom;

class CSRoomSorter
{
private:
    bool    _topDown;//if false: leftRightward
    
public:
    CSRoomSorter();
    CSRoomSorter(bool);
    
    void setSortMode(bool);
    
    bool operator()(CSRoom* &, CSRoom* &);
};

#endif /* CSRoomSorter_hpp */
