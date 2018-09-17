//
//  CSDungeonLevel.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/3/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include <fstream>
#include "CSDungeonLevel.hpp"
#include "CSRoomProx.hpp"
#include "CSDungObj.hpp"
#include "CSAxis.hpp"
#include "CSPoint.hpp"
#include "CSLine.hpp"

#pragma mark Constructors

CSDungeonLevel::CSDungeonLevel(CSRandomHandler *inRandHand, CSGameState *inGame, int inLevelNum)
{
    _theRandHand = inRandHand;
    _theGame = inGame;
    
    _roomComparator.setSortMode(false);
    
    _levelNum = inLevelNum;
    _fileName = "dungeonLevel";
    _fileName += to_string(_levelNum);
    _fileName += ".txt";
}


#pragma mark -
#pragma mark Doers - Dungeon Functions

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
        {
            _levelRooms.push_back(superRoom);
            _theGame->centerGameWindow(superRoom->getRect()->getCenterPoint());
        }
        else
            printf("Made a bad room. Type 'r' to try again.\n");
    }
    
    updateRoomNums();
    
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
#pragma mark Doers - Room Functions

void CSDungeonLevel::indexRooms(list<CSRoom*> *inRoomList)
{
    int loop = 0;
    
    list<CSRoom *>::iterator    listIter;
    
    inRoomList->sort(_roomComparator);
    
    for(listIter = inRoomList->begin(); listIter != inRoomList->end(); listIter++)
    {
        (*listIter)->setRoomNum(loop);
        loop++;
    }
}

void CSDungeonLevel::updateLevelBounds(CSRoom *inRoom)
{
    if(inRoom->getRect()->topLeft.x > _levelBounds.topLeft.x)
    {
        _levelBounds.topLeft.x = inRoom->getRect()->topLeft.x;
        _outerRooms[getIntFromReg(REG_WALL_LEFT)] = inRoom;
    }
    if(inRoom->getRect()->topLeft.y > _levelBounds.topLeft.y)
    {
        _levelBounds.topLeft.y = inRoom->getRect()->topLeft.y;
        _outerRooms[getIntFromReg(REG_WALL_TOP)] = inRoom;
    }
    if(inRoom->getRect()->botRight.x > _levelBounds.botRight.x)
    {
        _levelBounds.botRight.x = inRoom->getRect()->botRight.x;
        _outerRooms[getIntFromReg(REG_WALL_RIGHT)] = inRoom;
    }
    if(inRoom->getRect()->botRight.y > _levelBounds.botRight.y)
    {
        _levelBounds.botRight.y = inRoom->getRect()->botRight.y;
        _outerRooms[getIntFromReg(REG_WALL_BOT)] = inRoom;
    }
}

void CSDungeonLevel::updateRoomNums(void)
{
    int numRoomsRemainder = (int)_levelRooms.size() - 1, numDigits = 0;
    
    list<CSRoom *>::iterator    listIter;
    
    do
    {
        numRoomsRemainder /= 10;
        numDigits++;
    }
    while(numRoomsRemainder > 0);
    
    for(listIter = _levelRooms.begin(); listIter != _levelRooms.end(); listIter++)
        (*listIter)->updateRoomNum(numDigits);
}

CSRoom* CSDungeonLevel::createRoom(CSRoom *inRoom)
{
    bool        goodDoorLoc, forwardRangeWallReached, sideRangeWallReached;
    int         loop, newRandNum;
    objReg      newDoorWall = REG_NULL, nextDoorWall = REG_NULL, wallToCheck;
    CSAxis      roomGenAxis, paraWallAxis, rangeWallAxis;
    CSPoint     newPoint;
    CSLine      wallGenRange[NUM_ROOM_WALLS];
    CSRoom      *newRoom = new CSRoom(_theGame);
    CSDungObj   *inDoor = nullptr, *newDoor = new CSDungObj, *nextDoor = nullptr;
    
    vector<CSPoint>             rangeWallPoint;
    vector<CSPoint>::iterator   vectIter;
    list<CSRoom *>::iterator    roomListIter;
    list<CSDungObj *>::iterator objListIter;
    

    //if it's the seed room, generate a random width & height, surrounding the center of the level
    if(inRoom == nullptr)
    {
        //set the random ranges for the seed room
        CSRandomRange   roomSizeGen("roomSizeGen", ROOM_SIZE_MIN, ROOM_SIZE_MAX),
        hallLengthGen("hallLengthGen", ROOM_SIZE_MIN, HALL_LENGTH_MAX),
        roomSideGen("roomSideGen", REG_WALL_LEFT, REG_WALL_BOT);
        
        _theRandHand->addRandomRange(roomSizeGen);
        _theRandHand->addRandomRange(roomSideGen);
        _theRandHand->addRandomRange(hallLengthGen);
        
        //random location for the seed room
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
        nextDoorWall = getRegFromInt(_theRandHand->getNumber("roomSideGen"));
        CSRandomRange   doorLocGen("doorLocGen", newRoom->getRect()->getWallStartPoint(nextDoorWall) + 1, newRoom->getRect()->getWallEndPoint(nextDoorWall) - 1);//offsets keep doors from appearing in corners
        _theRandHand->addRandomRange(doorLocGen);
        
        //dynamically set the door's loc in the wall
        roomGenAxis.setAxisFromWall(nextDoorWall);
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newRoom->getRect()->getWallLocPoint(nextDoorWall));
        newPoint.setAxisPoint(roomGenAxis.dim, _theRandHand->getNumber("doorLocGen"));
        newRoom->createObject(OPEN_DOOR_CHAR, OBJ_DOOR, nextDoorWall, newPoint, nullptr, nextDoor);
        
        newRoom->setHallState(false);
        updateLevelBounds(newRoom);
        
        newRoom->setRoomNum((int)_levelRooms.size());
        return newRoom;
    }//if we haven't left the method yet...
    
    
    /*Create a door*/
    
    //loop through all inRoom's objects to find the door with no room yet connected
    for(objListIter = inRoom->getObjects()->begin(); objListIter != inRoom->getObjects()->end(); objListIter++)
        if((*objListIter)->getType() == OBJ_DOOR && (*objListIter)->getConnect() == nullptr)//this is a door & it should connect to our new room
        {
            inDoor = *objListIter;
            newDoorWall = getFacingWall(inDoor->getRegion());
            roomGenAxis.setAxisFromWall(inDoor->getRegion());//set the dim to HORIZ or VERT and the dir to UP_LEFT or DOWN_RIGHT
            
            break;//don't keep looping once we've made a room--one room per method call
        }
    
    //create a new door in our new room, adjacent to the door we found in inRoom, then connect it to inRoom
    newPoint = (*inDoor->getLoc());
    newPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), roomGenAxis.getDirectionOffset());//offset the appropriate axis 1 away, using roomGenAxis (e.g.: x-- or y-- OR x++ or y++)
    newRoom->addDoor(newDoor, newDoorWall, newPoint, nullptr, inDoor, newRoom);
    
    
    /*Set up the random ranges*/
    //set the upper gen range to max, then bring back in, if level walls are closer. Then bring in again if nearby rooms are closer. Thus, our new room can never overlap anything or go somewhere it shouldn't.
    
    //set up our ranges to go from newDoor, to the furthest away the new walls could be generated (based on ROOM_SIZE_MAX).
    for(loop = 0; loop < NUM_ROOM_WALLS; loop++)
    {
        //don't check backward into inRoom
        if(loop == getIntFromReg(newDoorWall))
            continue;
        
        rangeWallAxis.setAxisFromWall(getRegFromInt(loop));//set the dim to HORIZ or VERT and the dir to UP_LEFT or DOWN_RIGHT
        wallGenRange[loop].setStart(newPoint.getAxisPoint(rangeWallAxis.getPerpAxis()));//start from newDoor
        wallGenRange[loop].setEnd((rangeWallAxis.getAxisMod(ROOM_SIZE_MAX, PERPENDICULAR)) * rangeWallAxis.getDirectionOffset());//end positive or negative ROOM_SIZE_MAX from newDoor (though we start at just ROOM_SIZE_MAX)
        
        if(loop != getIntFromReg(getFacingWall(newDoorWall)))//if we're setting up either perp wall range...
        {
            wallGenRange[loop].setStart(wallGenRange[loop].getStart() + rangeWallAxis.getDirectionOffset());//the closest the perp walls can be is 1 away. Otherwise, hallways would open up into room's walls.
            wallGenRange[loop].setEnd(wallGenRange[loop].getEnd() / 2);//halve upper range since we're only generating 1/2 of the room
        }
        else//if we're setting up the opposite wall range...
            wallGenRange[loop].setStart(wallGenRange[loop].getStart() + (ROOM_SIZE_MIN * rangeWallAxis.getDirectionOffset()));//the closest the wall across from newDoor can be is ROOM_SIZE_MIN
                /*IF THAT'S NOT DOABLE, we have to switch modes, to create a hallway to an adjacent room*/
        
        wallGenRange[loop].setEnd(wallGenRange[loop].getEnd() + wallGenRange[loop].getStart());//now push it out by the line's start.
    }
    
    //check if the boundries of the level are closer to inDoor than the max room sizes -- make this a loop
    if(wallGenRange[getIntFromReg(REG_WALL_LEFT)].doesContain(0))
        wallGenRange[getIntFromReg(REG_WALL_LEFT)].setEnd(1);
    if(wallGenRange[getIntFromReg(REG_WALL_TOP)].doesContain(0))
        wallGenRange[getIntFromReg(REG_WALL_TOP)].setEnd(1);
    if(wallGenRange[getIntFromReg(REG_WALL_RIGHT)].doesContain(LEVEL_BOUND_RIGHT))
        wallGenRange[getIntFromReg(REG_WALL_RIGHT)].setEnd(LEVEL_BOUND_RIGHT - 1);
    if(wallGenRange[getIntFromReg(REG_WALL_BOT)].doesContain(LEVEL_BOUND_BOTTOM))
        wallGenRange[getIntFromReg(REG_WALL_BOT)].setEnd(LEVEL_BOUND_BOTTOM - 1);
    
    
    if(_theGame->getBreakState())
        loop = 0;//leave break point for debug purposes.
    
    
    /*find the nearest room's walls to inDoor, and set the random room range max's to be smaller than this*/
    
    //loop through each direction, skipping back toward previous room
    
        //get dir and dim for this direction, then loop through every room
    
            //get distance between inDoor and facingWall of room along the dim
    
            //if it's less than the currently dtored shortest distance, set shortest distance to it
    
        //if shortest distance is less than wallRange[this loop], set wallRange[this loop] to shortest distance
    
    
    /*first check para range from newDoor to set upper most para range (will likely end up being the same as ROOM_SIZE_MAX)
     
     then check the two perp ranges from newDoor to para range
     
     collect each change in perp range (getting smaller as it goes out)
     
     loop through each of those collected ranges
     
        check the entire para range from one perp range to the other for EACH collected per range in the loop
     
        if the resulting total perimeter (or area?) of the ranges in this configuration is the largest we have yet check in this loop
     
        save it as the best / set all ranges to this configuration
     */
    
    paraWallAxis.setAxisFromWall(getFacingWall(newDoorWall));//set the dim to HORIZ or VERT and the dir to UP_LEFT or DOWN_RIGHT, based on the wall opposite/para from newDoorWall. This will be used even as we check the perp walls
    
    for(loop = 0; loop <= 2; loop++)
    {
        sideRangeWallReached = false;//we need one outer while loop no matter which direction we're checking in
        
        //translate the loop into one para wall or the other
        if(loop == 0)
            wallToCheck = paraWallAxis.getReg();
        else if(loop == 1)
            wallToCheck = getClockWall(paraWallAxis.getReg());
        else
            wallToCheck = getCountclockWall(paraWallAxis.getReg());
        
        rangeWallAxis.setAxisFromWall(wallToCheck);//set the dim to HORIZ or VERT and the dir to UP_LEFT or DOWN_RIGHT, based on the wall we're checking this loop
        rangeWallPoint.clear();
        newPoint = *newDoor->getLoc();//set this perp rangeWall to newDoor
        newPoint.setAxisPoint(rangeWallAxis.getPerpAxis(), wallGenRange[getIntFromReg(wallToCheck)].getEnd());//move it out to this perp range wall
        rangeWallPoint.push_back(newPoint);//add it to the vector--it's the one range wall we know we'll need
        newPoint = *newDoor->getLoc();//set up the the point we'll be sliding out from center
        newPoint.setAxisPoint(rangeWallAxis.getPerpAxis(), wallGenRange[getIntFromReg(wallToCheck)].getStart());//move it out to where this wall gen range line starts (1 away from newDoor or ROOM_SIZE_MIN away from newDoor)
        
        while(!sideRangeWallReached)//while rangeWallPoint has not yet passed the opposite/para rangeWall, sideways
        {
            forwardRangeWallReached = false;
            
            while(!forwardRangeWallReached)//while rangeWallPoint has not yet passed the rangeWall
            {
                for(roomListIter = _levelRooms.begin(); roomListIter != _levelRooms.end(); roomListIter++)//loop through every room
                    if((*roomListIter)->getRect()->doesContain(newPoint))//if we've slide into roomListIter
                    {
                        newPoint.setAxisPoint(rangeWallAxis.getPerpAxis(),
                            (*roomListIter)->getRect()->getWallLocPoint(getFacingWall(rangeWallAxis.getReg())) -
                            (1 * rangeWallAxis.getDirectionOffset()));//set para rangeWall point one closer to newDoor from the room it was inside
                        forwardRangeWallReached = true;
                    }
                
                if(forwardRangeWallReached)
                    break;//if we found ourselves in another room, no need to do any further checks
                else
                    newPoint.slidePointViaAxis(rangeWallAxis.getPerpAxis(), (ROOM_SIZE_MIN * rangeWallAxis.getDirectionOffset()));//move away from newDoor toward the rangeWall by an increment of the smallest room size
                
                //if the distance between newPoint and newDoor is bigger than the distance between opposite/para rangeWall from newDoor, then we've gone past opposite/para's rangeWall and we can stop checking for other rooms
                if(abs(newPoint.getAxisPoint(rangeWallAxis.getPerpAxis()) -
                       newDoor->getLoc()->getAxisPoint(rangeWallAxis.getPerpAxis()))
                   > abs(wallGenRange[getIntFromReg(wallToCheck)].getEnd() -
                       newDoor->getLoc()->getAxisPoint(rangeWallAxis.getPerpAxis())))
                {
                    newPoint.setAxisPoint(rangeWallAxis.getPerpAxis(), wallGenRange[getIntFromReg(getFacingWall(newDoorWall))].getEnd());//set rangeWallPoint to be at opposite/para's rangeWall
                    forwardRangeWallReached = true;
                }
            }
            
            if(loop == 0)//we're checking opposite/para range wall
            {
                wallGenRange[getIntFromReg(getFacingWall(newDoorWall))].setEnd(newPoint.getAxisPoint(rangeWallAxis.getPerpAxis()));//we now know the furthest out our room's random wall placement can be without hitting another room, so we set the appropriate wallGenRange
                sideRangeWallReached = true;//only one outer while loop when checking toward opposite/para range wall
            }
            else
            {
                if(newPoint.getAxisPoint(rangeWallAxis.getPerpAxis()) != rangeWallPoint.back().getAxisPoint(rangeWallAxis.getPerpAxis()))//if the point where we hit a wall on this row is a new range wall point...
                    rangeWallPoint.push_back(newPoint);//add it to the vector
                
                newPoint.slidePointViaAxis(rangeWallAxis.dim, (ROOM_SIZE_MIN * paraWallAxis.getDirectionOffset()));//move away from newDoor toward the opposite/para range wall by an increment of the smallest room size, along the perp range wall we're checking this loop
                
                //if the distance between newPoint and newDoor is bigger than the distance between opposite/para rangeWall from newDoor, then we've gone past opposite/para's rangeWall and we can stop checking for other rooms
                if(abs(newPoint.getAxisPoint(rangeWallAxis.dim) -
                       newDoor->getLoc()->getAxisPoint(rangeWallAxis.dim))
                   > abs(wallGenRange[getIntFromReg(getFacingWall(newDoorWall))].getEnd() - newDoor->getLoc()->getAxisPoint(rangeWallAxis.dim)))
                    sideRangeWallReached = true;
            }
        }
        
        //now we go through every para range wall option and see how big a room we can make with that set up rangeWalls/wallGenRanges
        if(loop > 0)
            for(vectIter = rangeWallPoint.begin(); vectIter != rangeWallPoint.end(); vectIter++)
            {
                
            }
    }
    
    
    //now that we know our max's, set the random ranges accordingly
    CSRandomRange   roomSideGen("roomSideGen", REG_WALL_LEFT, REG_WALL_BOT),
                    wallLocGen[NUM_ROOM_WALLS];//the 4 walls
    
    for(loop = 0; loop < NUM_ROOM_WALLS; loop++)
    {
        //don't add the unused direction
        if(loop == getIntFromReg(newDoorWall))
            continue;
        
        wallLocGen[loop].setRangeMin(wallGenRange[loop].getStart());
        wallLocGen[loop].setRangeMax(wallGenRange[loop].getEnd());
        
        _theRandHand->addRandomRange(wallLocGen[loop]);
    }
    
    _theRandHand->addRandomRange(roomSideGen);
    
    
    /*Make the room*/
    
    newRoom->setHallState(!inRoom->isHall());//room gen alternates between rooms and hallways
    
    //dynamically generate a new rect, and create a room, from newDoor
    
    //set our facing wall adjacent to inRoom's door wall
    newPoint = *newDoor->getLoc();//start with newDoor
    if(newRoom->isHall())
        newPoint.slidePointViaAxis(roomGenAxis.dim, (HALL_SIZE / 2) * roomGenAxis.getOpposingDirOffset());//then slide parallel to newDoorWall
    else//non-hall room
    {
        //set perpendicular wall up to half room size away from door
        newRandNum = _theRandHand->getNumber(&wallLocGen[getIntFromReg(getAxisDirWall(newDoorWall))]);
        newPoint.setAxisPoint(roomGenAxis.dim, newRandNum);
    }
    newRoom->getRect()->setCorner(roomGenAxis.getOpposingDir(), newPoint);//create the closer corner of the new room
    
    if(newRoom->isHall())
    {
        //add or subtract hall length to get opposite wall
        newRandNum = _theRandHand->getNumber("hallLengthGen");
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newPoint.getAxisPoint(roomGenAxis.getPerpAxis()) -
                              ((roomGenAxis.getAxisMod(newRandNum, PERPENDICULAR)) * roomGenAxis.getOpposingDirOffset()));
        
        //set opposite, perpendicular wall adjacent to door the other way
        newPoint.setAxisPoint(roomGenAxis.dim, newPoint.getAxisPoint(roomGenAxis.dim) - (HALL_SIZE * roomGenAxis.getOpposingDirOffset()));
    }
    else//non-hall room
    {
        //generate opposite wall, from previously established random range
        newRandNum = _theRandHand->getNumber(&wallLocGen[getIntFromReg(getFacingWall(newDoorWall))]);
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newRandNum);
        
        //set opposite, perpendicular wall to half room size away from door the other way
        newRandNum = _theRandHand->getNumber(&wallLocGen[getIntFromReg(getAxisDirWall(getFacingWall(newDoorWall)))]);
        newPoint.setAxisPoint(roomGenAxis.dim, newRandNum);
    }
    newRoom->getRect()->setCorner(roomGenAxis.dir, newPoint);//create the further away corner of the new room
    
    
    /*Make a door*/
    
    //add a new door for the next iteration of createRoom
    
    if(newRoom->isHall())//to the opposite wall for a hallway
    {
        nextDoorWall = inDoor->getRegion();
        
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newRoom->getRect()->getWallLocPoint(nextDoorWall));
        newPoint.setAxisPoint(roomGenAxis.dim, newRoom->getRect()->topLeft.getAxisPoint(roomGenAxis.dim) + (HALL_SIZE / 2));
    }
    else//or to a random wall for a room
    {
        do
        {
            nextDoorWall = getRegFromInt(_theRandHand->getNumber("roomSideGen"));
            goodDoorLoc = true;
            
            //make sure it's a wall that has no door, and that adding a door now won't create problems with very nearby rooms, in the next iteration -- in the future, same door walls should be okay, within reason, so we need to create some check for distance minimum between same door walls.
            if(nextDoorWall == newDoorWall)
                goodDoorLoc = false;
        }
        while(!goodDoorLoc);
        
        //dynamically set the door loc to be along the chosen wall at a random point
        CSRandomRange   doorLocGen("doorLocGen", newRoom->getRect()->getWallStartPoint(nextDoorWall) + 1,
                                   newRoom->getRect()->getWallEndPoint(nextDoorWall) - 1);//offsets keep doors from appearing in corners
        _theRandHand->addRandomRange(doorLocGen);
        
        //dynamically set the door's loc in the wall
        roomGenAxis.setAxisFromWall(nextDoorWall);
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newRoom->getRect()->getWallLocPoint(nextDoorWall));
        newPoint.setAxisPoint(roomGenAxis.dim, _theRandHand->getNumber("doorLocGen"));
    }
    
    //final steps!
    newRoom->createObject(OPEN_DOOR_CHAR, OBJ_DOOR, nextDoorWall, newPoint, nullptr, nextDoor);
    newRoom->setRoomNum((int)_levelRooms.size());
    updateLevelBounds(newRoom);
    
    return newRoom;
}

void CSDungeonLevel::deleteLastRoom(void)
{
    list<CSRoom *>::iterator    listIter;
    
    listIter = _levelRooms.end();
    listIter--;
    
    _levelRooms.erase(listIter);
    (*listIter)->deleteRoom();
}

void CSDungeonLevel::slideRoom(int inRoomNum, int inXDist, int inYDist)//for development
{
    CSPoint slideVector(inXDist, inYDist);
    list<CSRoom *>::iterator    listIter;
    
    for(listIter = _levelRooms.begin(); listIter != _levelRooms.end(); listIter++)
        if((*listIter)->getRoomNum() == inRoomNum)
            break;
    
    (*listIter)->slideRoom(slideVector);
    
    //once the slide is complete, check for any collisions. If there are, do the inverse slide
}

#pragma mark
#pragma mark Doers - Graphics Functions

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

