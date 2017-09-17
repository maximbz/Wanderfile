//
//  CSDungObjSorter.hpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 6/17/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSDungObjSorter_hpp
#define CSDungObjSorter_hpp

#include <stdio.h>
#include "CSDungObj.hpp"

class CSDungObjSorter
{
public:
    CSDungObjSorter();
    
    bool operator()(CSDungObj* &, CSDungObj* &);
};

#endif /* CSDungObjSorter_hpp */
