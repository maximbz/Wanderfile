//
//  CSEntitySorter.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 6/17/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSEntitySorter.hpp"
#include "CSRect.hpp"
#include "CSAxis.hpp"

CSEntitySorter::CSEntitySorter()
{
    
}


bool CSEntitySorter::operator()(CSEntity* &earlierItem, CSEntity* &laterItem)
{
    CSAxis  earlierAxis, laterAxis;
    
    //shove all non-door entities to the back, doors of different walls should be in wall order
    if((earlierItem->getType() != laterItem->getType()))
        return earlierItem->getType() > laterItem->getType();
    else//if they're both doors of the same wall, compare axis points
        if(earlierItem->getType() == ENT_DOOR)
        {
            earlierAxis.setAxisFromWall(earlierItem->getRegion());
            laterAxis.setAxisFromWall(laterItem->getRegion());
            
            return earlierItem->getLoc()->getAxisPoint(earlierAxis.dim) < laterItem->getLoc()->getAxisPoint(laterAxis.dim);
        }
        else//if they're both the same non-door entity type, order doesn't (currently) matter
            return true;
}
