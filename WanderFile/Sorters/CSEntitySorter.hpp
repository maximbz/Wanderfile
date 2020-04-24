//
//  CSEntitySorter.hpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 6/17/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSEntitySorter_hpp
#define CSEntitySorter_hpp

#include <stdio.h>
#include "CSEntity.hpp"

class CSEntitySorter
{
public:
    CSEntitySorter();
    
    bool operator()(CSEntity* &, CSEntity* &);
};

#endif /* CSEntitySorter_hpp */
