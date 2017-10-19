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

void CSDungeonLevel::indexRooms(list<CSRoom*> *inRoomList)
{
    int loopCount = 0;
    
    list<CSRoom *>::iterator    listIter;
    
    inRoomList->sort(_roomComparator);
    
    for(listIter = inRoomList->begin(); listIter != inRoomList->end(); listIter++)
    {
        (*listIter)->setRoomNum(loopCount);
        loopCount++;
    }
}

void CSDungeonLevel::updateLevelBounds(CSRoom *inRoom)
{
    if(inRoom->getRect()->topLeft.x > _levelBounds.topLeft.x)
    {
        _levelBounds.topLeft.x = inRoom->getRect()->topLeft.x;
        _outerRooms[getIntFromRegion(REG_WALL_LEFT)] = inRoom;
    }
    if(inRoom->getRect()->topLeft.y > _levelBounds.topLeft.y)
    {
        _levelBounds.topLeft.y = inRoom->getRect()->topLeft.y;
        _outerRooms[getIntFromRegion(REG_WALL_TOP)] = inRoom;
    }
    if(inRoom->getRect()->botRight.x > _levelBounds.botRight.x)
    {
        _levelBounds.botRight.x = inRoom->getRect()->botRight.x;
        _outerRooms[getIntFromRegion(REG_WALL_RIGHT)] = inRoom;
    }
    if(inRoom->getRect()->botRight.y > _levelBounds.botRight.y)
    {
        _levelBounds.botRight.y = inRoom->getRect()->botRight.y;
        _outerRooms[getIntFromRegion(REG_WALL_BOT)] = inRoom;
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
    bool        goodDoorLoc;
    int         loopCount, newRandNum;
    objReg      originDoorWall = REG_NULL, newDoorWall = REG_NULL;
    CSAxis      roomGenAxis, wallAxis;
    CSPoint     newPoint;
    CSLine      wallGenLine[NUM_ROOM_WALLS];
    CSRange     nearestWallRange;
    CSRoom      *newRoom = new CSRoom(_theGame);
    CSDungObj   *inDoor = nullptr, *newDoor = new CSDungObj, *nextDoor = nullptr;
    
    vector<CSRoomProx>              nearbyRooms;
    vector<CSRoomProx>::iterator    vectIter;
    list<CSRoom *>::iterator        roomListIter;
    list<CSDungObj *>::iterator     objListIter;
    

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
        newDoorWall = getRegionFromInt(_theRandHand->getNumber("roomSideGen"));
        CSRandomRange   doorLocGen("doorLocGen", newRoom->getRect()->getWallStartPoint(newDoorWall) + 1, newRoom->getRect()->getWallEndPoint(newDoorWall) - 1);
        _theRandHand->addRandomRange(doorLocGen);//offsets keep doors from appearing in corners
        
        //dynamically set the door's loc in the wall
        roomGenAxis.setAxisFromWall(newDoorWall);
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newRoom->getRect()->getWallLocPoint(newDoorWall));
        newPoint.setAxisPoint(roomGenAxis.dim, _theRandHand->getNumber("doorLocGen"));
        newRoom->createObject(OPEN_DOOR_CHAR, OBJ_DOOR, newDoorWall, newPoint, nullptr, nextDoor);
        
        newRoom->setHallState(false);
        updateLevelBounds(newRoom);
        
        newRoom->setRoomNum((int)_levelRooms.size());
        return newRoom;
    }//if we haven't left the method yet...
    
    
    /*Set up the random ranges*/
    
    //loop through all inRoom's objects to find the door with no room yet connected
    for(objListIter = inRoom->getObjects()->begin(); objListIter != inRoom->getObjects()->end(); objListIter++)
        if((*objListIter)->getType() == OBJ_DOOR && (*objListIter)->getConnect() == nullptr)//this is a door & it should connect to our new room
        {
            inDoor = *objListIter;
            originDoorWall = getFacingWall(inDoor->getRegion());
            roomGenAxis.setAxisFromWall(inDoor->getRegion());//set the dim to HORIZ or VERT and the dir to UP_LEFT or DOWN_RIGHT
            
            break;//don't keep looping once we've made a room--one room per method call
        }
    
    //create a new door in our new room, adjacent to the door we found in inRoom, then connect it to inRoom
    newPoint = (*inDoor->getLoc());
    newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newPoint.getAxisPoint(roomGenAxis.getPerpAxis()) -
                          roomGenAxis.getOpposingDirOffset());//offset the appropriate axis 1 away, using roomGenAxis (e.g.: x-- or y-- OR x++ or y++)
    newRoom->addDoor(newDoor, originDoorWall, newPoint, nullptr, inDoor, newRoom);
    
    //set the upper gen range to max, then bring in if level walls are closer, then bring in again if nearby rooms are closer, so our new room can never overlap/go somewhere it shouldn't
    
    //set up our ranges to go from inDoor to the furthest away the new walls could be generated
    for(loopCount = 0; loopCount < NUM_ROOM_WALLS; loopCount++)
    {
        //don't check backward into inRoom
        if(loopCount == getIntFromRegion(originDoorWall))
            continue;
        
        wallAxis.setAxisFromWall(getRegionFromInt(loopCount));//set the dim to HORIZ or VERT and the dir to UP_LEFT or DOWN_RIGHT
        wallGenLine[loopCount].setStart(newPoint.getAxisPoint(wallAxis.getPerpAxis()) - wallAxis.getOpposingDirOffset());//x or y of this new room's originating door + at least 1 away from door (to match up with inRoom's wall)
        wallGenLine[loopCount].setEnd((wallAxis.getAxisMod(ROOM_SIZE_MAX, PERPENDICULAR)) * wallAxis.getDirectionOffset());//positive or negative ROOM_SIZE_MAX
        
        if(loopCount != getIntFromRegion(getFacingWall(originDoorWall)))//if it's to either side of the door (rather than across from the door)...
            wallGenLine[loopCount].setEnd(wallGenLine[loopCount].getEnd() / 2);//halve upper range since we're only generating 1/2 of the room
        else
            wallGenLine[loopCount].setStart(wallGenLine[loopCount].getStart() + (ROOM_SIZE_MIN * wallAxis.getDirectionOffset()));//closest wall across from newDoor can be is ROOM_SIZE_MIN
        
        wallGenLine[loopCount].setEnd(wallGenLine[loopCount].getEnd() + wallGenLine[loopCount].getStart());//gen line end is based on the pertinent originDoor's axis point
    }
    
    //check if the boundries of the level are closer to inDoor than the max room sizes
    if(wallGenLine[getIntFromRegion(REG_WALL_LEFT)].doesContain(0))
        wallGenLine[getIntFromRegion(REG_WALL_LEFT)].setEnd(1);
    if(wallGenLine[getIntFromRegion(REG_WALL_TOP)].doesContain(0))
        wallGenLine[getIntFromRegion(REG_WALL_TOP)].setEnd(1);
    if(wallGenLine[getIntFromRegion(REG_WALL_RIGHT)].doesContain(LEVEL_BOUND_RIGHT))
        wallGenLine[getIntFromRegion(REG_WALL_RIGHT)].setEnd(LEVEL_BOUND_RIGHT - 1);
    if(wallGenLine[getIntFromRegion(REG_WALL_BOT)].doesContain(LEVEL_BOUND_BOTTOM))
        wallGenLine[getIntFromRegion(REG_WALL_BOT)].setEnd(LEVEL_BOUND_BOTTOM - 1);
    
    //find the nearest room's walls to inDoor, and set the random room range max's to be smaller than this
    for(roomListIter = _levelRooms.begin(); roomListIter != _levelRooms.end(); roomListIter++)
    {
        for(loopCount = 0; loopCount < NUM_ROOM_WALLS; loopCount++)
        {
            //don't check backward into inRoom
            if(loopCount == getIntFromRegion(originDoorWall))
                continue;
            
            //check to see if inDoor aligns with a point within the facing wall
            wallAxis.setAxisFromWall(getRegionFromInt(loopCount));//set the dim to HORIZ or VERT and the dir to UP_LEFT or DOWN_RIGHT
            nearestWallRange = (*roomListIter)->getRect()->getWallRange(getFacingWall(getRegionFromInt(loopCount)));
            if(nearestWallRange.doesContain(newPoint.getAxisPoint(wallAxis.getWallAxis(getRegionFromInt(loopCount)))))
            {
                //if this is closest room to inDoor (in this direction) yet...
                if(wallGenLine[loopCount].doesContain((*roomListIter)->getRect()->getWallLocPoint(getFacingWall(getRegionFromInt(loopCount)))))
                    wallGenLine[loopCount].setEnd((*roomListIter)->getRect()->getWallLocPoint(getFacingWall(getRegionFromInt(loopCount))) -
                                                    wallAxis.getDirectionOffset());//move the wall gen max to 1 away from the nearets wall
            }
        }
    }
    
    
    if(_theGame->getBreakState())
        loopCount = 0;//leave break point for debug purposes.
    
    
    //now that we now our max's, set the random ranges accordingly
    CSRandomRange   roomSideGen("roomSideGen", REG_WALL_LEFT, REG_WALL_BOT),
                    wallLocGen[NUM_ROOM_WALLS];//the 4 walls
    
    for(loopCount = 0; loopCount < NUM_ROOM_WALLS; loopCount++)
    {
        //don't add the unused direction
        if(loopCount == getIntFromRegion(originDoorWall))
            continue;
        
        wallLocGen[loopCount].setRangeMin(wallGenLine[loopCount].getStart());
        wallLocGen[loopCount].setRangeMax(wallGenLine[loopCount].getEnd());
        
        _theRandHand->addRandomRange(wallLocGen[loopCount]);
    }
    
    _theRandHand->addRandomRange(roomSideGen);
    
    
    /*Make the room*/
    
    newRoom->setHallState(!inRoom->isHall());//room gen alternates between rooms and hallways
    
    //dynamically generate a new rect, and create a room, from newDoor
    
    newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newDoor->getLoc()->getAxisPoint(roomGenAxis.getPerpAxis()));//set our facing wall adjacent to inRoom's door wall
    if(newRoom->isHall())
    {
        //set perpendicular wall adjacent to door
        newPoint.setAxisPoint(roomGenAxis.dim, newDoor->getLoc()->getAxisPoint(roomGenAxis.dim) + ((HALL_SIZE / 2) * roomGenAxis.getOpposingDirOffset()));
    }
    else//non-hall room
    {
        //set perpendicular wall up to half room size away from door
        newRandNum = _theRandHand->getNumber(&wallLocGen[getIntFromRegion(getAxisDirWall(originDoorWall))]);
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
        newRandNum = _theRandHand->getNumber(&wallLocGen[getIntFromRegion(getFacingWall(originDoorWall))]);
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newRandNum);
        
        //set opposite, perpendicular wall to half room size away from door the other way
        newRandNum = _theRandHand->getNumber(&wallLocGen[getIntFromRegion(getAxisDirWall(getFacingWall(originDoorWall)))]);
        newPoint.setAxisPoint(roomGenAxis.dim, newRandNum);
    }
    newRoom->getRect()->setCorner(roomGenAxis.dir, newPoint);//create the further away corner of the new room
    
    
    /*Make a door*/
    
    //add a new door for the next iteration of createRoom
    
    if(newRoom->isHall())//to the opposite wall for a hallway
    {
        newDoorWall = inDoor->getRegion();
        
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newRoom->getRect()->getWallLocPoint(newDoorWall));
        newPoint.setAxisPoint(roomGenAxis.dim, newRoom->getRect()->topLeft.getAxisPoint(roomGenAxis.dim) + (HALL_SIZE / 2));
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
        }
        while(!goodDoorLoc);
        
        //dynamically set the door loc to be along the chosen wall at a random point
        CSRandomRange   doorLocGen("doorLocGen", newRoom->getRect()->getWallStartPoint(newDoorWall) + 1,
                                   newRoom->getRect()->getWallEndPoint(newDoorWall) - 1);
        _theRandHand->addRandomRange(doorLocGen);//offsets keep doors from appearing in corners
        
        //dynamically set the door's loc in the wall
        roomGenAxis.setAxisFromWall(newDoorWall);
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newRoom->getRect()->getWallLocPoint(newDoorWall));
        newPoint.setAxisPoint(roomGenAxis.dim, _theRandHand->getNumber("doorLocGen"));
    }
    
    //actually do the final steps!
    newRoom->createObject(OPEN_DOOR_CHAR, OBJ_DOOR, newDoorWall, newPoint, nullptr, nextDoor);
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

