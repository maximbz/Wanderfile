//
//  CSDungObjSorter.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 6/17/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSDungObjSorter.hpp"
#include "CSRect.hpp"

CSDungObjSorter::CSDungObjSorter()
{
    
}


bool CSDungObjSorter::operator()(CSDungObj* &earlierItem, CSDungObj* &laterItem)
{
    //shove all non-door objects to the back, doros of different walls should be in wall order
    if((earlierItem->getType() != laterItem->getType()))
        return earlierItem->getType() < laterItem->getType();
    else//if they're both doors of the same wall, compare axis points
        if(earlierItem->getType() == OBJ_TYPE_DOOR)
            return earlierItem->getLoc()->getAxisPoint(getWallAxis(earlierItem->getRegion())) < laterItem->getLoc()->getAxisPoint(getWallAxis(laterItem->getRegion()));
        else//if they're both the same non-door object type, order doesn't (currently) matter
            return true;
}
