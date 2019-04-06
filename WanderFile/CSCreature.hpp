//
//  CSCreature.hpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 4/3/19.
//  Copyright © 2019 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSCreature_hpp
#define CSCreature_hpp

#include <stdio.h>
#include "CSPoint.hpp"

class CSCreature
{
private:
    bool    _player;
    CSPoint _loc;
    
public:
    CSCreature();
    CSCreature(bool, CSPoint);
    
    bool moveCreature(CSPoint);
    
    void setIsPlayer(bool);
    void setLoc(CSPoint *);
    
    bool getIsPlayer(void);
    CSPoint* getLoc(void);
};

#endif /* CSCreature_hpp */
