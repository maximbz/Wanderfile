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
#include "CSDungObj.hpp"

class CSCreature
{
private:
    bool        _player;
    CSDungObj   _creatureObj;
    
public:
    CSCreature();
    CSCreature(bool, CSPoint *);
    
    void moveCreature(CSPoint *);
    
    void setIsPlayer(bool);
    void setLoc(CSPoint *);
    
    bool getIsPlayer(void);
    CSPoint* getLoc(void);
    CSDungObj* getCreatureObj(void);
};

#endif /* CSCreature_hpp */
