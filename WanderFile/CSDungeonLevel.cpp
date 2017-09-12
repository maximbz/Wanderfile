
//
//  CSDungeonLevel.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/3/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include <fstream>
#include "CSDungeonLevel.hpp"
#include "CSPoint.hpp"

#pragma mark Constructors

CSDungeonLevel::CSDungeonLevel(CSRandomHandler *incomingRandHand, CSGameState *incomingGame, int incomingLevelNum)
{
    _theRandHand = incomingRandHand;
    CSRandomRange   roomSizeGen("roomSizeGen", ROOM_SIZE_MIN, ROOM_SIZE_MAX),
                    hallLengthGen("hallLengthGen", ROOM_SIZE_MIN, HALL_LENGTH_MAX),
                    roomSideGen("roomSideGen", REG_WALL_LEFT, REG_WALL_BOT);
    
    _theRandHand->addRandomRange(roomSizeGen);
    _theRandHand->addRandomRange(roomSideGen);
    _theRandHand->addRandomRange(hallLengthGen);
    /*_theRandomHandler->addRandomRange(roomLocXGen);
    _theRandomHandler->addRandomRange(roomLocYGen);*/
    
    _theGame = incomingGame;
    
    _roomComparator.setSortMode(false);
    
    _levelNum = incomingLevelNum;
    _fileName = "dungeonLevel";
    _fileName += to_string(_levelNum);
    _fileName += ".txt";
}


#pragma mark -
#pragma mark Doers

void CSDungeonLevel::createDungeon(void)
{
    CSRoom  *superRoom, *subRoom;
    //bool  makeRooms = true;
    
    list<CSRoom *>::iterator    listIter;
    
    //seed the dungeon with one room
    if(_levelRooms.empty())
        _levelRooms.push_back(createRoom(nullptr));
    else
    {
        subRoom = _levelRooms.back();
        superRoom = createRoom(subRoom);
        
        if(superRoom != nullptr)
            _levelRooms.push_back(superRoom);
        else
            printf("Made a bad room. Type 'r' to try again.\n");
    }
    
    /*do
    {
        subRoom = createRoom(superRoom);
        _levelRooms.push_back(subRoom);
        superRoom = subRoom;
        
        if(_levelRooms.size() >= NUM_ROOMS)
            makeRooms = false;
    }
    while(makeRooms);*/
    
    //indexRooms(&_levelRooms);
}

int CSDungeonLevel::saveDungeon(void)
{
    ofstream    outputFile;
    
    list<CSRoom *>::iterator    listIter;
    
    outputFile.open(_fileName.c_str(), ios_base::app);
    
    if(outputFile.fail())
    {
        perror(_fileName.c_str());
        return 1;
    }
    
    for(listIter = _levelRooms.begin(); listIter != _levelRooms.end(); listIter++)
        outputFile << (*listIter)->printRoomToFile();
    
    outputFile.close();
    
    return 0;
}

int CSDungeonLevel::loadDungeon(void)
{
    ifstream    inputFile;
    string      inputString, levelData = "";
    
    inputFile.open(_fileName);
    if(inputFile.fail())
    {
        perror(_fileName.c_str());
        return 1;
    }
    
    while(getline(inputFile, inputString))
    {
        levelData += inputString;
        levelData += "\n";
    }
    
    return 0;
}

void CSDungeonLevel::deleteDungeon(void)
{
    list<CSRoom *>::iterator    listIter;
    
    for(listIter = _levelRooms.begin(); listIter != _levelRooms.end(); listIter++)
        (*listIter)->deleteRoom();
    _levelRooms.clear();
    
    _levelBounds.setPoints(BAD_DATA, BAD_DATA, BAD_DATA, BAD_DATA);
}

#pragma mark -

void CSDungeonLevel::indexRooms(list<CSRoom*> *incomingRoomList)
{
    int loopCounter = 0;
    
    list<CSRoom *>::iterator    listIter;
    
    incomingRoomList->sort(_roomComparator);
    
    for(listIter = incomingRoomList->begin(); listIter != incomingRoomList->end(); listIter++)
    {
        (*listIter)->setRoomNum(loopCounter);
        loopCounter++;
    }
}

void CSDungeonLevel::updateLevelBounds(CSRoom *incomingRoom)
{
    if(incomingRoom->getRect()->topLeft.x > _levelBounds.topLeft.x)
        _levelBounds.topLeft.x = incomingRoom->getRect()->topLeft.x;
    if(incomingRoom->getRect()->topLeft.y > _levelBounds.topLeft.y)
        _levelBounds.topLeft.y = incomingRoom->getRect()->topLeft.y;
    if(incomingRoom->getRect()->botRight.x > _levelBounds.botRight.x)
        _levelBounds.botRight.x = incomingRoom->getRect()->botRight.x;
    if(incomingRoom->getRect()->botRight.y > _levelBounds.botRight.y)
        _levelBounds.botRight.y = incomingRoom->getRect()->botRight.y;
}

CSRoom* CSDungeonLevel::createRoom(CSRoom *incomingRoom)
{
    CSDungObj   *incomingDoor = nullptr, *newDoor = new CSDungObj, *nextDoor = nullptr;
    CSRoom      *newRoom = new CSRoom(_theGame);
    bool        goodDoorLoc, nextWall = false;
    int         axis = BAD_DATA, dir = BAD_DATA, loopCounter, thisIndexDist, nextIndexDist;
    objReg      originDoorWall = REG_NULL, newDoorWall = REG_NULL;
    CSPoint     newPoint;
    
    
    
    wallOverlap overlapWall[NUM_ROOM_WALLS], tempOverlap;
    
    vector<CSRoomProx>              nearbyRooms;
    vector<CSRoomProx>::iterator    vectIter;
    list<CSRoom *>::iterator        roomListIter;
    list<CSDungObj *>::iterator     objListIter;
    

    //if it's the seed room, generate a random width & height, surrounding the center of the level
    if(incomingRoom == nullptr)
    {
        //random locatino for the seed room
        newPoint.x = (LEVEL_BOUND_RIGHT / 2) - (_theRandHand->getNumber("roomSizeGen") / 2);
        newPoint.y = (LEVEL_BOUND_BOTTOM / 2) - ((_theRandHand->getNumber("roomSizeGen") / 2) / 2);//rooms look taller than they are because of ascii output, so we halve room height gens to make squarer looking rooms
        newRoom->getRect()->setTopLeft(newPoint);
        
        //ranodm size for the seed room
        newPoint.x = _theRandHand->getNumber("roomSizeGen");
        newPoint.x += newRoom->getRect()->topLeft.x;
        newPoint.y = _theRandHand->getNumber("roomSizeGen") / 2;//halve height due to asymmetry of ascii output
        newPoint.y += newRoom->getRect()->topLeft.y;
        newRoom->getRect()->setBotRight(newPoint);
        
        //make a door on any side
        newDoorWall = getRegionFromInt(_theRandHand->getNumber("roomSideGen"));
        CSRandomRange   doorLocGen("doorLocGen", newRoom->getRect()->getWallStartPoint(newDoorWall) + 1, newRoom->getRect()->getWallEndPoint(newDoorWall) - 1);
        _theRandHand->addRandomRange(doorLocGen);//offsets keep doors from appearing in corners
        
        //dynamically set the door's loc in the wall
        axis = !getWallAxis(newDoorWall);
        newPoint.setAxisPoint(axis, newRoom->getRect()->getWallLocPoint(newDoorWall));
        newPoint.setAxisPoint(!axis, _theRandHand->getNumber("doorLocGen"));
        newRoom->createDoor(newDoorWall, newPoint, nextDoor);
        
        newRoom->setHallState(false);
        updateLevelBounds(newRoom);
        
        newRoom->setRoomNum((int)_levelRooms.size());
        return newRoom;
    }//if we haven't left the method yet...
    
    
    /*Make the room*/
    
    //generate the new room's rect, attached to the incomingRoom's unconnected door
    newRoom->setHallState(!incomingRoom->isHall());//alternate between rooms and hallways
    
    //loop through all incomingRoom's objects to find the door with no room yet connected
    for(objListIter = incomingRoom->getObjects()->begin(); objListIter != incomingRoom->getObjects()->end(); objListIter++)
        if((*objListIter)->getType() == OBJ_TYPE_DOOR && (*objListIter)->getConnect() == nullptr)//this is a door & it should connect to our new room
        {
            //set the dim to HORIZ or VERT and the dir to UP_LEFT or DOWN_RIGHT
            axis = !getWallAxis((*objListIter)->getRegion());
            dir = getWallDir((*objListIter)->getRegion()) * -1;
            
            incomingDoor = *objListIter;
            originDoorWall = getFacingWall(incomingDoor->getRegion());
            
            break;//don't keep looping once we've made a room--one room per method call
        }
            
    //dynamically create a room from the unconnected door we found
    newPoint.setAxisPoint(axis, incomingDoor->getLoc()->getAxisPoint(axis) - dir);//set our facing wall adjacent to incomingRoom's door wall
    if(newRoom->isHall())
        newPoint.setAxisPoint(!axis, incomingDoor->getLoc()->getAxisPoint(!axis) + ((HALL_SIZE / 2) * dir));//set perpendicular wall adjacent to door
    else
        newPoint.setAxisPoint(!axis, incomingDoor->getLoc()->getAxisPoint(!axis) + (((_theRandHand->getNumber("roomSizeGen") / (!axis + 1)) / 2) * dir));//set perpendicular wall to half room size away from door
    newRoom->getRect()->setCorner(dir, newPoint);//create the closer corner of the new room
    
    if(newRoom->isHall())
    {
        newPoint.setAxisPoint(axis, newPoint.getAxisPoint(axis) - ((_theRandHand->getNumber("hallLengthGen") / (axis + 1)) * dir));//add or subtract hall length to get opposite wall
        newPoint.setAxisPoint(!axis, newPoint.getAxisPoint(!axis) - (HALL_SIZE * dir));//set opposite, perpendicular wall adjacent to door the other way
    }
    else
    {
        newPoint.setAxisPoint(axis, newPoint.getAxisPoint(axis) - ((_theRandHand->getNumber("roomSizeGen") / (axis + 1)) * dir));//add or subtract room size to get opposite wall
        newPoint.setAxisPoint(!axis, incomingDoor->getLoc()->getAxisPoint(!axis) - (((_theRandHand->getNumber("roomSizeGen") / (axis + 1)) / 2) * dir));//set opposite, perpendicular wall to half room size away from door the other way
    }
    newRoom->getRect()->setCorner(dir * -1, newPoint);//create the further away corner of the new room
    
    //create a new door in our new room, adjacent to the door we found in incomingRoom, then connect it to incomingRoom
    newPoint = (*incomingDoor->getLoc());
    newPoint.setAxisPoint(axis, newPoint.getAxisPoint(axis) - dir);//x-- or y-- OR x++ or y++
    newRoom->addDoor(newDoor, originDoorWall, newPoint, incomingDoor, newRoom);
    incomingDoor->setConnectTo(newDoor);//and add us to them
    
    
    /*Check for collisions*/
    
    //check if the new room is beyond the level bounds, and if so, move it in
    if(newRoom->getRect()->topLeft.x <= 0)
        newRoom->getRect()->topLeft.x = 1;
    if(newRoom->getRect()->topLeft.y <= 0)
        newRoom->getRect()->topLeft.y = 1;
    if(newRoom->getRect()->botRight.x >= LEVEL_BOUND_RIGHT)
        newRoom->getRect()->botRight.x = LEVEL_BOUND_RIGHT - 1;
    if(newRoom->getRect()->botRight.y >= LEVEL_BOUND_BOTTOM)
        newRoom->getRect()->botRight.y = LEVEL_BOUND_BOTTOM - 1;
    
    //check if the new room is within any other room
    for(loopCounter = 0; loopCounter < NUM_ROOM_WALLS; loopCounter++)//we're using the y coord to record which wall and the x coord to record the point
    {
        overlapWall[loopCounter].wall = getRegionFromInt(loopCounter);
        overlapWall[loopCounter].distance = BAD_DATA;
    }
    
    if(doesRoomOverlap(newRoom, overlapWall))//this line populates the overlapWall array
    {
        //we can only ever allow the un-attached end of a hallways to be adjusted
        if(newRoom->isHall())
        {
            newRoom->getRect()->setWallLoc(incomingDoor->getRegion(), overlapWall[getIntFromRegion(incomingDoor->getRegion())].wall);//hall can only ever be adjusted to end adjacent to neighboring room
            
            //we still need to create a door on the room we just attatched to, and a connection
        }
        else
        {
            //order each new wall loc by smallest distance from current wall loc
            for(loopCounter = 0; loopCounter < (NUM_ROOM_WALLS - 1); loopCounter++)
            {
                //the distance between the current wall and the suggested new wall loc
                thisIndexDist = abs(overlapWall[loopCounter + 1].wall - newRoom->getRect()->getWallLocPoint(loopCounter));
                nextIndexDist = abs(overlapWall[loopCounter].wall - newRoom->getRect()->getWallLocPoint(loopCounter));
                
                if(nextIndexDist < thisIndexDist)//if the index below this one is smaller
                {
                    //move it up
                    tempOverlap = overlapWall[loopCounter];
                    overlapWall[loopCounter] = overlapWall[loopCounter +1];
                    overlapWall[loopCounter + 1] = tempOverlap;
                    
                    //the index we just slid up may have to slide up again, so let's check the previous iteration of this loop
                    if(loopCounter > 0)//we're not on the first index
                        loopCounter--;//we may need to keep sliding this index up the rankings
                }
            }
            
            //move down the ordered list and choose a wall to move
            for(loopCounter = 0; loopCounter < NUM_ROOM_WALLS; loopCounter++)
            {
                if(overlapWall[loopCounter].distance == getFacingWall(incomingDoor->getRegion()))//if this index is the wall with incomingDoor in it
                    nextWall = true;
                else if(overlapWall[loopCounter].distance == getClockwiseWall(incomingDoor->getRegion()) || overlapWall[loopCounter].distance == getFacingWall(getClockwiseWall(incomingDoor->getRegion())))//if it's either perpendicular wall
                {
                    if((incomingRoom->getRect()->getWallEndPoint(incomingDoor->getRegion()) > overlapWall[loopCounter].wall &&
                        overlapWall[loopCounter].distance == REG_WALL_BOT) ||//if hall's max is greater than new bottom,
                       (incomingRoom->getRect()->getWallStartPoint(incomingDoor->getRegion()) < overlapWall[loopCounter].wall &&
                        overlapWall[loopCounter].distance == REG_WALL_TOP))//OR if hall's min is less than new top
                        nextWall = true;//then NO, we can't slide the wall that far
                    else//if the perpendicular walls are the shortest distance to slide the offending/overlapping wall that don't have some room-breaking issue, let's slide them!
                        newRoom->getRect()->setWallLoc(overlapWall[loopCounter].distance, overlapWall[loopCounter].wall);
                }
                else//it's the far wall from incomingDoor, so there should be no issues moving it
                {
                    //if sliding the wall opposite the door to incomingRoom pushes beyond that door, then we move to the next index or stop trying to make this room
                    if((getWallDir(overlapWall[loopCounter].wall) == DOWN_RIGHT &&
                       overlapWall[loopCounter].wall < newRoom->getRect()->botRight.getAxisPoint(getWallDir(overlapWall[loopCounter].wall) * -1)) ||
                       (getWallDir(overlapWall[loopCounter].wall) == DOWN_RIGHT &&
                        overlapWall[loopCounter].wall < newRoom->getRect()->botRight.getAxisPoint(getWallDir(overlapWall[loopCounter].wall) * -1)))
                        nextWall = true;
                    else
                        newRoom->getRect()->setWallLoc(overlapWall[loopCounter].distance, overlapWall[loopCounter].wall);
                }
                
                if(nextWall)
                {
                    //we move down the index, or if we're at the bottom, we give up on this room/set ourselves up to slide the entire room
                    if(loopCounter < NUM_ROOM_WALLS - 1)
                    {
                        nextWall = false;
                        continue;
                    }
                    else
                    {
                        delete newDoor;
                        delete newRoom;
                        
                        return nullptr;
                    }
                }
            }
        }
    }
    
    
    /*Make a door*/
    
    //add a new door for the next iteration of createRoom
    if(newRoom->isHall())//to the opposite wall for a hallway
    {
        newDoorWall = incomingDoor->getRegion();
        
        newPoint.setAxisPoint(axis, newRoom->getRect()->getWallLocPoint(newDoorWall));
        newPoint.setAxisPoint(!axis, newRoom->getRect()->topLeft.getAxisPoint(!axis) + (HALL_SIZE / 2));
    }
    else//or to a random wall for a room
    {
        do
        {
            newDoorWall = getRegionFromInt(_theRandHand->getNumber("roomSideGen"));
            goodDoorLoc = true;
            
            //make sure it's a wall that has no door, and that adding a door now won't create problems with very nearby rooms, in the next iteration
            if(newDoorWall == originDoorWall)
                goodDoorLoc = false;
            /*else//if we think this wall works, let's make sure to first check how close the nearest room is
            {
                shortestRoomDist = LEVEL_BOUND_RIGHT * LEVEL_BOUND_BOTTOM;
                nearbyRooms = getNearbyRooms(newRoom, newDoorWall);
                
                for(vectIter = nearbyRooms.begin(); vectIter != nearbyRooms.end(); vectIter++)
                {
                    if((*vectIter).getNearbyRoom() == incomingRoom)
                        continue;
                    
                    if((*vectIter).getDistance() < shortestRoomDist)
                        shortestRoomDist = (*vectIter).getDistance();
                }
                
                if(shortestRoomDist <= ROOM_DISTANCE_MIN)
                    goodDoorLoc = false;
            }*/
        }
        while(!goodDoorLoc);
        
        //dynamically set the door loc to be along the chosen wall at a random point
        CSRandomRange   doorLocGen("doorLocGen", newRoom->getRect()->getWallStartPoint(newDoorWall) + 1, newRoom->getRect()->getWallEndPoint(newDoorWall) - 1);
        _theRandHand->addRandomRange(doorLocGen);//offsets keep doors from appearing in corners
        
        //dynamically set the door's loc in the wall
        axis = !getWallAxis(newDoorWall);
        newPoint.setAxisPoint(axis, newRoom->getRect()->getWallLocPoint(newDoorWall));
        newPoint.setAxisPoint(!axis, _theRandHand->getNumber("doorLocGen"));
    }
    
    newRoom->createDoor(newDoorWall, newPoint, nextDoor);
    
    //number it and expand the level bounds, and add it to the list
    updateLevelBounds(newRoom);
    newRoom->setRoomNum((int)_levelRooms.size());
    return newRoom;
}

bool CSDungeonLevel::doesRoomOverlap(CSRoom *incomingRoom, wallOverlap incOverlapWalls[NUM_ROOM_WALLS])//checks incoming wall of incoming room against all other rooms
{
    bool        overlap = false;
    int         loopCounter;
    CSRange     incPerpWallRange, incWallRange, facePerpWallRange, faceWallRange;
    
    list<CSRoom *>::iterator    listIter;
    
    for(listIter = _levelRooms.begin(); listIter != _levelRooms.end(); listIter++)
    {
        for(loopCounter = 0; loopCounter < NUM_ROOM_WALLS; loopCounter++)
        {
            //set our room's dims (width & height or height & width)
            incPerpWallRange.setRange(incomingRoom->getRect()->getWallStartPoint(loopCounter),
                                      incomingRoom->getRect()->getWallEndPoint(loopCounter));
            incWallRange.setRange(incomingRoom->getRect()->getWallLocPoint(loopCounter),
                         incomingRoom->getRect()->getWallLocPoint(getFacingWall(getRegionFromInt(loopCounter))));
            
            //set listIter's dims
            facePerpWallRange.setRange((*listIter)->getRect()->getWallStartPoint(getFacingWall(getRegionFromInt(loopCounter))),
                                       (*listIter)->getRect()->getWallEndPoint(getFacingWall(getRegionFromInt(loopCounter))));
            faceWallRange.setRange((*listIter)->getRect()->getWallLocPoint(getFacingWall(getRegionFromInt(loopCounter))),
                                     (*listIter)->getRect()->getWallLocPoint(loopCounter));
            
            //check to see if our room aligns perpedicularly and parallelly with listIter (and is thus inside it)
            if(incPerpWallRange.max >= facePerpWallRange.min && incPerpWallRange.min <= facePerpWallRange.max)
                if(((incWallRange.min - faceWallRange.max) <= 0) && ((faceWallRange.min - incWallRange.max) <= 0))
                {
                    incOverlapWalls[loopCounter].distance = (*listIter)->getRect()->getWallLocPoint(getFacingWall(getRegionFromInt(loopCounter))) -
                    getWallDir(getRegionFromInt(loopCounter));//return a wall loc that will be just outside of listIter
                    overlap = true;
                }
        }
        
        if(overlap)
            return true;
    }
    
    return false;//incomingRoom->getRect()->getWallLocPoint(incomingWall);//if no collisions are found, return incoming wall loc, unchanged
}

vector<CSRoomProx> CSDungeonLevel::getNearbyRooms(CSRoom *incomingRoom, objReg incomingWall)
{
    objReg      facingWall = getFacingWall(incomingWall);
    CSRange     incomingWallRange(incomingRoom->getRect()->getWallStartPoint(incomingWall),
                                  incomingRoom->getRect()->getWallEndPoint(incomingWall)),facingWallRange;
    CSRoomProx  outgoingProx;
    
    vector<CSRoomProx>          proxVect;
    list<CSRoom *>::iterator    listIter;
    
    for(listIter = _levelRooms.begin(); listIter != _levelRooms.end(); listIter++)
    {
        facingWallRange.setRange((*listIter)->getRect()->getWallStartPoint(facingWall), (*listIter)->getRect()->getWallEndPoint(facingWall));
        if(facingWallRange.max >= incomingWallRange.min && facingWallRange.min <= incomingWallRange.max)
        {
            outgoingProx.setNearbyRoom(*listIter);
            outgoingProx.setDistance(abs((*listIter)->getRect()->getWallLocPoint(facingWall) - incomingRoom->getRect()->getWallLocPoint(incomingWall)));
            
            //overlap range must be where both rooms overlap
            outgoingProx.setRoomOverlap(facingWallRange);
            if(incomingWallRange.min > facingWallRange.min)
                outgoingProx.setRoomOverlapMin(incomingWallRange.min);
            if(incomingWallRange.max < facingWallRange.max)
                outgoingProx.setRoomOverlapMax(incomingWallRange.max);
            
            proxVect.push_back(outgoingProx);
        }
    }
    
    return proxVect;
}

void CSDungeonLevel::deleteLastRoom(void)
{
    list<CSRoom *>::iterator    listIter;
    
    listIter = _levelRooms.end();
    listIter--;
    
    _levelRooms.erase(listIter);
    (*listIter)->deleteRoom();
}

void CSDungeonLevel::slideRoom(int incomingRoomNum, int incomingXDist, int incomingYDist)//for development
{
    CSPoint slideVector(incomingXDist, incomingYDist);
    list<CSRoom *>::iterator    listIter;
    
    for(listIter = _levelRooms.begin(); listIter != _levelRooms.end(); listIter++)
        if((*listIter)->getRoomNum() == incomingRoomNum)
            break;
    
    (*listIter)->slideRoom(slideVector);
    
    //once the slide is complete, check for any collisions. If there are, do the inverse slide
}

#pragma mark

void CSDungeonLevel::printWindow()
{
    //draw from file, based on visable range

    CSPoint charToPrint;
    CSRange printRange;
    int     spaceCounter;
    
    list<CSRoom *>              windowRooms, rowRooms;
    list<CSRoom *>::iterator    listIter;
    
    if(_theGame->getBreakState())
        spaceCounter = 0;//leave a break point here for debug
    
    //loop through all of the Dungeon's Rooms and if any part of them is within the window, add them to the windowRooms vector
    for(listIter = _levelRooms.begin(); listIter != _levelRooms.end(); listIter++)
        if(((*listIter)->getRect()->topLeft.y <= _theGame->getGameWindow().botRight.y && (*listIter)->getRect()->botRight.y >= _theGame->getGameWindow().topLeft.y) && ((*listIter)->getRect()->topLeft.x <= _theGame->getGameWindow().botRight.x && (*listIter)->getRect()->botRight.x >= _theGame->getGameWindow().topLeft.x))
            windowRooms.push_back(*listIter);
    
    //slide down from the top of the window to the bottom
    for(charToPrint.y = _theGame->getGameWindow().topLeft.y; charToPrint.y <= _theGame->getGameWindow().botRight.y; charToPrint.y++)
    {
        //loop through windowRooms and determine if any are on this row
        for(listIter = windowRooms.begin(); listIter != windowRooms.end(); listIter++)
            if((*listIter)->getRect()->topLeft.y <= charToPrint.y && (*listIter)->getRect()->botRight.y >= charToPrint.y)//if they are, add them to the vector for this row
               rowRooms.push_back(*listIter);
        
        rowRooms.sort(_roomComparator);
        
        charToPrint.x = _theGame->getGameWindow().topLeft.x;
        
        //loop through rowRooms and draw them, starting with the space before them
        for(listIter = rowRooms.begin(); listIter != rowRooms.end(); listIter++)
        {
            for(spaceCounter = 0; spaceCounter < (*listIter)->getRect()->topLeft.x - charToPrint.x; spaceCounter++)
                printf(" ");
            
            printRange.setRange(_theGame->getGameWindow().topLeft.x, _theGame->getGameWindow().botRight.x);
            printf("%s", (*listIter)->printRoomRow(printRange, charToPrint.y).c_str());
            charToPrint.x = (*listIter)->getRect()->botRight.x + 1;//move to the other side of the room to continue the row
        }
        
        rowRooms.clear();
        printf("\n");
    }
}


#pragma mark -
#pragma mark Getters

int CSDungeonLevel::getLevelNumber(void)
{
    return _levelNum;
}

int CSDungeonLevel::getNumRooms(void)
{
    return (int)_levelRooms.size();
}

