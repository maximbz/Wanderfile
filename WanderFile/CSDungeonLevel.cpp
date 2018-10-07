//
//  CSDungeonLevel.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/3/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include <fstream>
#include "CSRandomList.hpp"
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
    list<CSRoom *>::iterator    listIter = _levelRooms.begin();
    
    while(listIter != _levelRooms.end())
    {
        (*listIter)->deleteRoom();
        listIter = _levelRooms.erase(listIter);//new iterator properly goes through the list, now with fewer entries
    }
    _levelRooms.clear();
    
    _levelBounds.setPoints(BAD_DATA, BAD_DATA, BAD_DATA, BAD_DATA);
    
    _theRandHand->clearRandomItems(RAND_DUNGEON);
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
    bool            goodDoorLoc, roomIntersects[NUM_ROOM_WALLS];
    int             loop, newRandNum;
    objReg          newDoorWall = REG_NULL, nextDoorWall = REG_NULL, loopReg, regToSlide;
    CSAxis          roomGenAxis, paraWallAxis, rangeRectAxis;
    CSPoint         newPoint;
    CSRect          roomGenRect[NUM_ROOM_WALLS + 1];//center and each wall
    CSRoom          *newRoom = new CSRoom(_theGame);
    CSDungObj       *inDoor = nullptr, *newDoor = new CSDungObj, *nextDoor = nullptr;
    CSRandomRange   roomSideGen(RAND_DUNGEON, REG_WALL_LEFT, REG_WALL_BOT), hallLengthGen(RAND_DUNGEON, ROOM_SIZE_MIN, HALL_LENGTH_MAX),
                    wallLocGen[NUM_ROOM_WALLS];//the 4 walls
    
    list<CSRoom *>::iterator    roomListIter;
    list<CSDungObj *>::iterator objListIter;

    //whether the seed room or not, we'll need to add these ranges to _theRandHand
    _theRandHand->addRandomRange(roomSideGen);
    _theRandHand->addRandomRange(hallLengthGen);
    for(loop = 0; loop < NUM_ROOM_WALLS; loop++)
        wallLocGen[loop].setRandType(RAND_DUNGEON);
    
    //if it's the seed room, generate a random width & height, surrounding the center of the level
    if(inRoom == nullptr)
    {
        //set the random range for the seed room
        CSRandomRange   roomSizeGen(RAND_DUNGEON, ROOM_SIZE_MIN, ROOM_SIZE_MAX);
        _theRandHand->addRandomRange(roomSizeGen);
        
        //random location for the seed room
        newPoint.x = (LEVEL_BOUND_RIGHT / 2) - (_theRandHand->getNumber(&roomSizeGen) / 2);
        newPoint.y = (LEVEL_BOUND_BOTTOM / 2) - ((_theRandHand->getNumber(&roomSizeGen) / 2) / 2);//rooms look taller than they are because of ascii output, so we halve room height gens to make squarer looking rooms
        newRoom->getRect()->setTopLeft(newPoint);
        
        //ranodm size for the seed room
        newPoint.x = _theRandHand->getNumber(&roomSizeGen);
        newPoint.x += newRoom->getRect()->topLeft.x;
        newPoint.y = _theRandHand->getNumber(&roomSizeGen) / 2;//halve height due to asymmetry of ascii output
        newPoint.y += newRoom->getRect()->topLeft.y;
        newRoom->getRect()->setBotRight(newPoint);
        
        //make a door on any side
        nextDoorWall = getRegFromInt(_theRandHand->getNumber(&roomSideGen));
        CSRandomRange   doorLocGen(RAND_DUNGEON, newRoom->getRect()->getWallStartPoint(nextDoorWall) + 1, newRoom->getRect()->getWallEndPoint(nextDoorWall) - 1);//offsets keep doors from appearing in corners
        _theRandHand->addRandomRange(doorLocGen);
        
        //dynamically set the door's loc in the wall
        roomGenAxis.setAxisFromWall(nextDoorWall);
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newRoom->getRect()->getWallLocPoint(nextDoorWall));
        newPoint.setAxisPoint(roomGenAxis.dim, _theRandHand->getNumber(&doorLocGen));
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
            roomGenAxis.setAxisFromWall(newDoorWall);//set the dim to HORIZ or VERT and the dir to UP_LEFT or DOWN_RIGHT
            
            break;//don't keep looping once we've made a room--one room per method call
        }
    
    //create a new door in our new room, adjacent to the door we found in inRoom, then connect it to inRoom
    newPoint = (*inDoor->getLoc());
    newPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), roomGenAxis.getOppDirOffset());//offset the appropriate axis 1 away, using roomGenAxis (e.g.: x-- or y-- OR x++ or y++)
    newRoom->addDoor(newDoor, newDoorWall, newPoint, nullptr, inDoor, newRoom);
    
    
    /*Set up the region rects that act as random ranges*/
    
    //set the outer wall to room size max, then bring back in, if level walls are closer. Then bring in again if nearby rooms are closer. Thus, our new room can never overlap anything or go somewhere it shouldn't.
    
    //first we create the min room size rect, based on newDoor.loc
    
    newPoint = (*newDoor->getLoc());
    newPoint.slidePointViaAxis(roomGenAxis.dim, (ROOM_SIZE_MIN / 2) * roomGenAxis.getDirOffset());//slide along newDoorWall to create the rects's nearest point (to the right of newDoor, from the perspective of someone walking into newRoom)
    roomGenRect[getIntFromReg(REG_ROOM_RECT_MIN)].setCorner(roomGenAxis.dir, newPoint);
    
    //add or subtract ROOM_SIZE_MIN to get opposite wall
    newPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), ROOM_SIZE_MIN * roomGenAxis.getOppDirOffset());
    //set opposite, perpendicular wall adjacent to newDoor the other way (to the left of newDoor and across the room, from the perspective of someone walking into newRoom)
    newPoint.setAxisPoint(roomGenAxis.dim, newPoint.getAxisPoint(roomGenAxis.dim) - (ROOM_SIZE_MIN * roomGenAxis.getDirOffset()));
    roomGenRect[getIntFromReg(REG_ROOM_RECT_MIN)].setCorner(roomGenAxis.getOppDir(), newPoint);
    
    //check here if any room intersects. If so, check to see if newDoor can be placed in the offending room. If it can, attach inDoor's owner (previous hallway) to offending room
    //if newDoor cannot be placed in the offending room, delete newDoor, delete newRoom, and tell inDoor's owner (previous room) to delete itself too. (This should auto-delete previous room's attached door in previous previous room)
    //either way, send a message back to createDungeon() to start on a new door
    
    
    //then we make the three region rects around the min room size rect
    
    loop = getIntFromReg(roomGenAxis.getPerpReg());//perp reg attatched by dir from newDoorWall
    newPoint = (*newDoor->getLoc());
    newPoint.slidePointViaAxis(roomGenAxis.dim, ((ROOM_SIZE_MIN / 2) * roomGenAxis.getDirOffset()) - roomGenAxis.getOppDirOffset());
    newPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), roomGenAxis.getAxisMod(ROOM_SIZE_MAX, PERP) * roomGenAxis.getOppDirOffset());
    roomGenRect[loop].setCorner(roomGenAxis.getOppDir(), newPoint);
    newPoint = (*newDoor->getLoc());
    newPoint.slidePointViaAxis(roomGenAxis.dim, (roomGenAxis.getAxisMod(ROOM_SIZE_MAX, PARA) / 2) * roomGenAxis.getDirOffset());
    //perpdim doesn't need to be changed
    roomGenRect[loop].setCorner(roomGenAxis.dir, newPoint);
    
    if(roomGenRect[loop].getHeight() < 0 || roomGenRect[loop].getWidth() < 0)
        loop = 0;
    
    loop = getIntFromReg(getFacingWall(roomGenAxis.getReg()));//across from newDoorWall
    newPoint = (*newDoor->getLoc());
    newPoint.slidePointViaAxis(roomGenAxis.dim, (roomGenAxis.getAxisMod(ROOM_SIZE_MAX, PARA) / 2) * roomGenAxis.getOppDirOffset());
    newPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), roomGenAxis.getAxisMod(ROOM_SIZE_MAX, PERP) * roomGenAxis.getOppDirOffset());
    roomGenRect[loop].setCorner(roomGenAxis.getOppDir(), newPoint);
    newPoint = (*newDoor->getLoc());
    newPoint.slidePointViaAxis(roomGenAxis.dim, (roomGenAxis.getAxisMod(ROOM_SIZE_MAX, PARA) / 2) * roomGenAxis.getDirOffset());
    newPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), (ROOM_SIZE_MIN * roomGenAxis.getOppDirOffset()) - roomGenAxis.getDirOffset());
    roomGenRect[loop].setCorner(roomGenAxis.dir, newPoint);
    
    if(roomGenRect[loop].getHeight() < 0 || roomGenRect[loop].getWidth() < 0)
        loop = 0;
    
    loop = getIntFromReg(getFacingWall(roomGenAxis.getPerpReg()));//opposite reg from perp reg attatched by dir from newDoorWall
    newPoint = (*newDoor->getLoc());
    newPoint.slidePointViaAxis(roomGenAxis.dim, ((roomGenAxis.getAxisMod(ROOM_SIZE_MAX, PARA) / 2) * roomGenAxis.getOppDirOffset()) - roomGenAxis.getDirOffset());
    newPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), roomGenAxis.getAxisMod(ROOM_SIZE_MAX, PERP) * roomGenAxis.getOppDirOffset());
    roomGenRect[loop].setCorner(roomGenAxis.getOppDir(), newPoint);
    newPoint = (*newDoor->getLoc());
    newPoint.slidePointViaAxis(roomGenAxis.dim, ((ROOM_SIZE_MIN / 2) * roomGenAxis.getOppDirOffset()));
    //perpdim doesn't need to be changed
    roomGenRect[loop].setCorner(roomGenAxis.dir, newPoint);

    if(roomGenRect[loop].getHeight() < 0 || roomGenRect[loop].getWidth() < 0)
        loop = 0;
    
    
    //check if the boundries of the level are closer to inDoor than the max room sizes -- make this a loop
    /*if(wallGenRange[getIntFromReg(REG_WALL_LEFT)].doesContain(0))
        wallGenRange[getIntFromReg(REG_WALL_LEFT)].setEnd(1);
    if(wallGenRange[getIntFromReg(REG_WALL_TOP)].doesContain(0))
        wallGenRange[getIntFromReg(REG_WALL_TOP)].setEnd(1);
    if(wallGenRange[getIntFromReg(REG_WALL_RIGHT)].doesContain(LEVEL_BOUND_RIGHT))
        wallGenRange[getIntFromReg(REG_WALL_RIGHT)].setEnd(LEVEL_BOUND_RIGHT - 1);
    if(wallGenRange[getIntFromReg(REG_WALL_BOT)].doesContain(LEVEL_BOUND_BOTTOM))
        wallGenRange[getIntFromReg(REG_WALL_BOT)].setEnd(LEVEL_BOUND_BOTTOM - 1);*/
    
    /*check region zero: if there's a room in it, abort this room gen
        make a method for handling that, where the previous hallways connects to the offending room
            hallway or room can slide to join up
            if that won't work, hallways is deleted, along with connecting door from previous room
                new path is started from previous room (make a "find a good room" method)*/
        
    //check this reg's rect
    for(roomListIter = _levelRooms.begin(); roomListIter != _levelRooms.end(); roomListIter++)
    {
        for(loop = 0; loop < NUM_ROOM_WALLS; loop++)//clear the bool array
            roomIntersects[loop] = false;
        regToSlide = REG_NULL;
        
        newRandNum = (*roomListIter)->getRoomNum();//for debug
        if(_theGame->getBreakState())
            loop = 0;//leave break point for debug purposes.
        
        for(loop = 0; loop < NUM_ROOM_WALLS; loop++)//check each region rect
        {
            if(loop == getIntFromReg(newDoorWall))//don't check the backward into newDoor
                continue;
            
            //any reg rects that this room intersects, set true
            if(roomGenRect[loop].doesRectContainWall(*(*roomListIter)->getRect(), getFacingWall(getRegFromInt(loop))))
            {
                regToSlide = getRegFromInt(loop);//if only one reg intersects, we'll catch it here
                roomIntersects[loop] = true;//if multiple reg's intersect, we decide below, and overwrite the previous line
            }
        }
        
        //if multiple reg rects intersect the room, we determine the one reg rect we'll adjust
        
        if(roomIntersects[getIntFromReg(getFacingWall(newDoorWall))] && roomIntersects[getIntFromReg(getClockWall(newDoorWall))] && roomIntersects[getIntFromReg(getCountclockWall(newDoorWall))])//if the room intersects all 3 reg rects...
            regToSlide = getFacingWall(newDoorWall);//we only adjust oppositeWall
        else if(roomIntersects[getIntFromReg(getFacingWall(newDoorWall))] && roomIntersects[getIntFromReg(getClockWall(newDoorWall))])//if the room intersects a corner...
        {
            //decide randomly between the two rects that form that corner
            CSRandomList   roomGenRectList(RAND_DUNGEON);
            roomGenRectList.addNumToList(getIntFromReg(getFacingWall(newDoorWall)));
            roomGenRectList.addNumToList(getIntFromReg(getClockWall(newDoorWall)));
            _theRandHand->addRandomList(roomGenRectList);
            
            regToSlide = getRegFromInt(_theRandHand->getNumber(&roomGenRectList));
        }
        else if(roomIntersects[getIntFromReg(getFacingWall(newDoorWall))] && roomIntersects[getIntFromReg(getCountclockWall(newDoorWall))])//if the room intersects the other corner...
        {
            //decide randomly between the two rects that form that corner
            CSRandomList   roomGenRectList(RAND_DUNGEON);
            roomGenRectList.addNumToList(getIntFromReg(getFacingWall(newDoorWall)));
            roomGenRectList.addNumToList(getIntFromReg(getCountclockWall(newDoorWall)));
            _theRandHand->addRandomList(roomGenRectList);
            
            regToSlide = getRegFromInt(_theRandHand->getNumber(&roomGenRectList));
        }
        
        //move rect wall inward
        if(regToSlide != REG_NULL)
        {
            rangeRectAxis.setAxisFromWall(regToSlide);
            roomGenRect[getIntFromReg(regToSlide)].setWallLoc(regToSlide, (*roomListIter)->getRect()->getWallLocPoint(getFacingWall(regToSlide)) - rangeRectAxis.getDirOffset());
            
            if(roomGenRect[getIntFromReg(regToSlide)].getDim(rangeRectAxis.dim) < 0)//if we just pushed rect's wall to the other side of its facing wall... (which should only happen if the offending room's wall is just outside roomGenRect[REG_ROOM_RECT_MIN])
                roomGenRect[getIntFromReg(regToSlide)].setWallLoc(getFacingWall(regToSlide), roomGenRect[getIntFromReg(regToSlide)].getWallLocPoint(regToSlide));//make both walls the same, forcing newRoom's wall to be on the edge of REG_ROOM_RECT_MIN/next to newDoor.
        }
    }
    
    
    //now that we know our max's, set the random ranges accordingly
    for(loop = 0; loop < NUM_ROOM_WALLS; loop++)
    {
        //don't add the unused direction
        if(loop == getIntFromReg(newDoorWall))
            continue;
        
        loopReg = getRegFromInt(loop);
        wallLocGen[loop].setRangeMin(roomGenRect[loop].getWallLocPoint(getFacingWall(loopReg)));//inner wall
        wallLocGen[loop].setRangeMax(roomGenRect[loop].getWallLocPoint(loopReg));//outer wall
        
        _theRandHand->addRandomRange(wallLocGen[loop]);
    }
    
    
    /*Make the room*/
    
    newRoom->setHallState(!inRoom->isHall());//room gen alternates between rooms and hallways
    
    //dynamically generate a new rect, and create a room, from newDoor
    
    //set our new wall to newDoor's loc
    newPoint = *newDoor->getLoc();//start with newDoor's loc
    if(newRoom->isHall())
        newPoint.slidePointViaAxis(roomGenAxis.dim, (HALL_SIZE / 2) * roomGenAxis.getDirOffset());//then slide along newDoorWall to create the room's nearest point (to the right of newDoor, from the perspective of someone walking into newRoom)
    else//non-hall room
    {
        //set perpendicular wall (to the right of newDoor, from the perspective of someone walking into newRoom) up to half room size away from newDoor
        newRandNum = _theRandHand->getNumber(&wallLocGen[getIntFromReg(roomGenAxis.getPerpReg())]);
        newPoint.setAxisPoint(roomGenAxis.dim, newRandNum);
    }
    newRoom->getRect()->setCorner(roomGenAxis.dir, newPoint);//create the closer corner of the new room
    
    if(newRoom->isHall())
    {
        //add or subtract hall length to get opposite wall
        newRandNum = _theRandHand->getNumber(&hallLengthGen);
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newPoint.getAxisPoint(roomGenAxis.getPerpAxis()) -
                              ((roomGenAxis.getAxisMod(newRandNum, PERP)) * roomGenAxis.getDirOffset()));
        
        //set opposite, perpendicular wall adjacent to newDoor the other way (to the left of newDoor and across the room, from the perspective of someone walking into newRoom)
        newPoint.setAxisPoint(roomGenAxis.dim, newPoint.getAxisPoint(roomGenAxis.dim) - (HALL_SIZE * roomGenAxis.getDirOffset()));
    }
    else//non-hall room
    {
        //generate opposite wall, from previously established random range
        newRandNum = _theRandHand->getNumber(&wallLocGen[getIntFromReg(getFacingWall(newDoorWall))]);
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newRandNum);
        
        //set opposite, perpendicular wall (to the left of newDoor and across the room, from the perspective of someone walking into newRoom) to half room size away from door, the other way
        newRandNum = _theRandHand->getNumber(&wallLocGen[getIntFromReg(getFacingWall(roomGenAxis.getPerpReg()))]);
        newPoint.setAxisPoint(roomGenAxis.dim, newRandNum);
    }
    newRoom->getRect()->setCorner(roomGenAxis.getOppDir(), newPoint);//create the further away corner of the new room
    
    
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
            nextDoorWall = getRegFromInt(_theRandHand->getNumber(&roomSideGen));
            goodDoorLoc = true;
            
            //make sure it's a wall that has no door, and that adding a door now won't create problems with very nearby rooms, in the next iteration -- in the future, same door walls should be okay, within reason, so we need to create some check for distance minimum between same door walls.
            if(nextDoorWall == newDoorWall)
                goodDoorLoc = false;
        }
        while(!goodDoorLoc);
        
        //dynamically set the door loc to be along the chosen wall at a random point
        CSRandomRange   doorLocGen(RAND_DUNGEON, newRoom->getRect()->getWallStartPoint(nextDoorWall) + 1,
                                   newRoom->getRect()->getWallEndPoint(nextDoorWall) - 1);//offsets keep doors from appearing in corners
        _theRandHand->addRandomRange(doorLocGen);
        
        //dynamically set the door's loc in the wall
        roomGenAxis.setAxisFromWall(nextDoorWall);
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newRoom->getRect()->getWallLocPoint(nextDoorWall));
        newPoint.setAxisPoint(roomGenAxis.dim, _theRandHand->getNumber(&doorLocGen));
    }
    
    //final steps!
    newRoom->createObject(OPEN_DOOR_CHAR, OBJ_DOOR, nextDoorWall, newPoint, nullptr, nextDoor);
    newRoom->setRoomNum((int)_levelRooms.size());
    updateLevelBounds(newRoom);
    
    //clean up
    _theRandHand->clearRandomItems(RAND_DUNGEON);
    
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

