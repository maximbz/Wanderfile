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

CSDungeonLevel::CSDungeonLevel(CSRandomHandler *inRandHand, CSGameState *inGame, CSDoorHandler *inDoorHand, int inLevelNum)
{
    _theRandHand = inRandHand;
    _theGame = inGame;
    _theDoorHand = inDoorHand;
    
    _roomComparator.setSortMode(false);//used for printWindow
    _levelNum = inLevelNum;
    _maxNumDoors = NUM_DOORS_MIN + ((_levelNum - 1) * NUM_DOORS_LEVEL_RATE);
    
    _fileName = "dungeonLevel";
    _fileName += to_string(_levelNum);
    _fileName += ".txt";
}


#pragma mark -
#pragma mark Doers - Dungeon Functions

void CSDungeonLevel::createDungeon(void)
{
    bool            goodRoom, makeRooms = true;
    CSRoom          *newRoom;
    CSDungObj       *nextDoor;
    int             newDoorNumQty[4] = {0,0,0,0};
    
    _dungeonBounds.setPoints((_theGame->getLevelBounds().botRight.x / 2), (_theGame->getLevelBounds().botRight.y / 2), -(_theGame->getLevelBounds().botRight.x / 2), (_theGame->getLevelBounds().botRight.y / 2));
    
    while(makeRooms)
    {
        //seed the dungeon with one room
        if(_levelRooms.empty())
            _levelRooms.push_back(createFirstRoom());
        else
        {
            newRoom = new CSRoom(_theGame, _theRandHand, _theDoorHand);
            nextDoor = _theDoorHand->getNextDoor();
            
            goodRoom = createRoomGenRanges(nextDoor, newRoom);//set up the random bounds of a new room
            if(goodRoom)//if that worked...
                goodRoom = createNewRoom(nextDoor, newRoom, newDoorNumQty);//make the room
            
            if(goodRoom)//if that also worked, add it to the room list
            {
                _levelRooms.push_back(newRoom);
                _theGame->centerGameWindow(newRoom->getRect()->getCenterPoint());
            }
            else
                deleteRoom(newRoom);
        }
        
        //clean up room-creating
        _theRandHand->clearRandomItems(RAND_ROOM);
        //updateRoomNums();
        
        if(_theDoorHand->getNumDoors() == 0)//if we're out of unconnected doors...
        {
            if(_levelRooms.size() < _maxNumDoors)//but we haven't hit level min...
                createOuterDoor();//create a new door on the outer edge of the dungeon, on the side closest to the center
            else
                makeRooms = false;//otherwise, we can leave the room creation loop
        }
    }
    
    //populate dungeon!
    createStairs();//add stairs based on top-most and bottom-most rooms
    createTreasure();
    //createMonsters();
    
    //clean up dungeon-creation
    _theRandHand->clearRandomItems(RAND_DUNGEON);
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
    _dungeonBounds.setPoints(BAD_DATA, BAD_DATA, -BAD_DATA, -BAD_DATA);
    _theRandHand->clearRandomItems(RAND_DUNGEON);
    _theDoorHand->clear();
}

#pragma mark -
#pragma mark Doers - Room Functions

/*void CSDungeonLevel::indexRooms(list<CSRoom*> *inRoomList)
{
    int loop = 0;
    
    list<CSRoom *>::iterator    listIter;
    
    inRoomList->sort(_roomComparator);
    
    for(listIter = inRoomList->begin(); listIter != inRoomList->end(); listIter++)
    {
        (*listIter)->setRoomNum(loop);
        loop++;
    }
}*/

void CSDungeonLevel::updateDungeonBounds(CSRoom *inRoom)
{
    if(inRoom->getRect()->topLeft.x < _dungeonBounds.topLeft.x)
    {
        _dungeonBounds.topLeft.x = inRoom->getRect()->topLeft.x;
        _outerRooms[(int)REG_WALL_LEFT] = inRoom;
    }
    if(inRoom->getRect()->topLeft.y < _dungeonBounds.topLeft.y)
    {
        _dungeonBounds.topLeft.y = inRoom->getRect()->topLeft.y;
        _outerRooms[(int)REG_WALL_TOP] = inRoom;
    }
    if(inRoom->getRect()->botRight.x > _dungeonBounds.botRight.x)
    {
        _dungeonBounds.botRight.x = inRoom->getRect()->botRight.x;
        _outerRooms[(int)REG_WALL_RIGHT] = inRoom;
    }
    if(inRoom->getRect()->botRight.y > _dungeonBounds.botRight.y)
    {
        _dungeonBounds.botRight.y = inRoom->getRect()->botRight.y;
        _outerRooms[(int)REG_WALL_BOT] = inRoom;
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

CSRoom* CSDungeonLevel::createFirstRoom(void)
{
    CSPoint         newPoint;
    CSAxis          roomGenAxis;
    CSRoom          *newRoom = new CSRoom(_theGame, _theRandHand, _theDoorHand);
    CSRandomRange   roomSideGen(RAND_DUNGEON, REG_WALL_LEFT, REG_WALL_BOT);
    
    //set the random range for the seed room
    CSRandomRange   roomSizeGen(RAND_DUNGEON, ROOM_SIZE_MIN * 2, ROOM_SIZE_MAX);
    _theRandHand->addRandomRange(roomSizeGen);
    
    //random location for the seed room
    newPoint.x = (_theGame->getLevelBounds().botRight.x / 2) - (_theRandHand->getNumber(&roomSizeGen) / 2);
    newPoint.y = (_theGame->getLevelBounds().botRight.y / 2) - ((_theRandHand->getNumber(&roomSizeGen) / 2) / 2);//rooms look taller than they are because of ascii output, so we halve room height gens to make squarer looking rooms
    newRoom->getRect()->setTopLeft(newPoint);
    
    //random size for the seed room
    newPoint.x = _theRandHand->getNumber(&roomSizeGen);
    newPoint.x += newRoom->getRect()->topLeft.x;
    newPoint.y = _theRandHand->getNumber(&roomSizeGen) / 2;//halve height due to asymmetry of ascii output
    newPoint.y += newRoom->getRect()->topLeft.y;
    newRoom->getRect()->setBotRight(newPoint);
    
    //finishing touches
    newRoom->setHallState(false);
    newRoom->createNewDoor(REG_NULL);
    newRoom->setRoomNum((int)_levelRooms.size());
    updateDungeonBounds(newRoom);
    
    return newRoom;
}

bool CSDungeonLevel::createRoomGenRanges(CSDungObj *inDoor, CSRoom *newRoom)
{
    bool            roomIntersects[NUM_ROOM_WALLS];
    int             loop, roomToConnectDist = BAD_DATA;
    objReg          newDoorWall = REG_NULL, loopReg, regToSlide[2];//includes a fallback reg
    CSAxis          roomGenAxis, rangeRectAxis;
    CSPoint         newPoint, newDoorPoint;
    CSLine          distToRoom;
    CSRect          roomGenRect[NUM_ROOM_WALLS + 1];//center and each wall
    CSRoom          *inRoom;
    CSRandomRange   roomSideGen(RAND_ROOM, REG_WALL_LEFT, REG_WALL_BOT), hallLengthGen(RAND_ROOM, ROOM_SIZE_MIN, HALL_LENGTH_MAX);
    
    list<CSRoom *>::iterator    roomListIter;
    
    
    /*Setup our origin point*/
    
    //create a new location that will be the door in our new room, adjacent to the door we found in inRoom, then connect it to inRoom -- this point is continually referenced in room generation
    
    if(_theGame->getBreakState())
        loop = 0;//leave break point for debug purposes.
    
    if(inDoor == nullptr)
        return false;
    
    newDoorWall = getFacingWall(inDoor->getRegion());
    roomGenAxis.setAxisFromWall(newDoorWall);//set the dim to HORIZ or VERT and the dir to UP_LEFT or DOWN_RIGHT
    newDoorPoint = (*inDoor->getLoc());
    newDoorPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), roomGenAxis.getOppDirOffset());//offset the appropriate axis 1 away, using roomGenAxis (e.g.: x-- or y-- OR x++ or y++)
    inRoom = inDoor->getOwner();
    newRoom->setHallState(!inRoom->isHall());//room gen alternates between rooms and hallways
    
    
    /*Set up the region rects that act as random ranges*/
    
    //if we're making a non-hallway:set the outer wall to room size max, then bring back in, if level walls are closer. Then bring in again if nearby rooms are closer. Thus, our new room can never overlap anything or go somewhere it shouldn't.
    //if we're making a hallway: set the one rect to the hallway max size, and if a room intersects it, the hallway changes modes to connect directly to that room. If we're outside level bounds, we cancel this hallway entirely
    
    //create the min room size rect, based on newDoor.loc, which determines whether a room can even be here in the first place.
    newPoint = newDoorPoint;
    if(!inRoom->isHall())//if newRoom is a hallway
        newPoint.slidePointViaAxis(roomGenAxis.dim, (HALL_SIZE / 2) * roomGenAxis.getDirOffset());//slide along newDoorWall to create the rects's nearest point (to the right of newDoor, from the perspective of someone walking into newRoom)
    else//if the new room is a normal room
        newPoint.slidePointViaAxis(roomGenAxis.dim, (ROOM_SIZE_MIN / 2) * roomGenAxis.getDirOffset());//slide along newDoorWall to create the rects's nearest point (to the right of newDoor, from the perspective of someone walking into newRoom)
    roomGenRect[(int)REG_ROOM_CORE].setCorner(roomGenAxis.dir, newPoint);
    
    if(!inRoom->isHall())//if newRoom is a hallway
    {
        newPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), HALL_LENGTH_MAX * roomGenAxis.getOppDirOffset());//add or subtract HALL_LENGTH_MAX to get opposite wall
        newPoint.setAxisPoint(roomGenAxis.dim, newPoint.getAxisPoint(roomGenAxis.dim) - (HALL_SIZE * roomGenAxis.getDirOffset()));//set opposite, perpendicular wall adjacent to newDoor the other way (to the left of newDoor and across the room, from the perspective of someone walking into newRoom)
    }
    else//if the new room is a normal room
    {
        newPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), ROOM_SIZE_MIN * roomGenAxis.getOppDirOffset());//add or subtract ROOM_SIZE_MIN to get opposite wall
        newPoint.setAxisPoint(roomGenAxis.dim, newPoint.getAxisPoint(roomGenAxis.dim) - (ROOM_SIZE_MIN * roomGenAxis.getDirOffset()));//set opposite, perpendicular wall adjacent to newDoor the other way (to the left of newDoor and across the room, from the perspective of someone walking into newRoom)
    }
    roomGenRect[(int)REG_ROOM_CORE].setCorner(roomGenAxis.getOppDir(), newPoint);
    
    //check if the boundries of the level are closer to inDoor than the min room size
    for(loop = 0; loop < NUM_ROOM_WALLS; loop++)//check each region rect
    {
        if(loop == (int)newDoorWall)//don't check the backward into newDoor
            continue;
        
        rangeRectAxis.setAxisFromWall((objReg)loop);
        
        if(!_theGame->getLevelBounds().getWallRange(getClockWall((objReg)loop)).doesContain(roomGenRect[(int)REG_ROOM_CORE].getWallLocPoint((objReg)loop)))//if the core rect is beyond level bounds...
        {
            //abort room gen--destroy this room and if this one wasn't a hallway, the previous one (and destroy the door that leads to that hallway)
            abortRoomGenPath(inRoom);
            return false;
        }
    }
    
    //check every existing room's opposite wall to see if one intersects with the "core-rect"
    for(roomListIter = _levelRooms.begin(); roomListIter != _levelRooms.end(); roomListIter++)
        if(roomGenRect[REG_ROOM_CORE].doesRectContainWall(*(*roomListIter)->getRect(), newDoorWall))//if it intersect with any room...
        {
            //set up a range to see how close this offending room is from newRoom's door
            rangeRectAxis.setAxisFromWall(getFacingWall(newDoorWall));
            distToRoom.setStart(newDoorPoint.getAxisPoint(rangeRectAxis.getPerpAxis()));
            distToRoom.setEnd((*roomListIter)->getRect()->getWallLocPoint(newDoorWall));
            
            //if roomListIter is closer to newRoom's door than a previous offending room, then it becomes the new roomToConnect.
            if(newRoom->getRoomToConnect() == nullptr || (newRoom->getRoomToConnect() != nullptr && distToRoom.getSize() < roomToConnectDist))
            {
                newRoom->setRoomToConnect(*roomListIter);
                roomToConnectDist = distToRoom.getSize();
            }
        }
    
    
    if(newRoom->isHall())//none of the rest of this concerns hallways, so we out
        return true;
    
    
    //then, if we're making a non-hallways/normal room, we make the three region rects around the min room size rect (though the outer walls of min rect overlap with the inner walls of the room gen rects)
    
    loop = (int)roomGenAxis.getPerpReg();//perp reg attatched by dir from newDoorWall
    newPoint = newDoorPoint;
    newPoint.slidePointViaAxis(roomGenAxis.dim, ((ROOM_SIZE_MIN / 2) * roomGenAxis.getDirOffset()));
    newPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), roomGenAxis.getAxisMod(ROOM_SIZE_MAX, PERP) * roomGenAxis.getOppDirOffset());
    roomGenRect[loop].setCorner(roomGenAxis.getOppDir(), newPoint);
    newPoint = newDoorPoint;
    newPoint.slidePointViaAxis(roomGenAxis.dim, (roomGenAxis.getAxisMod(ROOM_SIZE_MAX, PARA) / 2) * roomGenAxis.getDirOffset());
    //perpdim doesn't need to be changed
    roomGenRect[loop].setCorner(roomGenAxis.dir, newPoint);
    
    loop = (int)getFacingWall(roomGenAxis.getReg());//across from newDoorWall
    newPoint = newDoorPoint;
    newPoint.slidePointViaAxis(roomGenAxis.dim, (roomGenAxis.getAxisMod(ROOM_SIZE_MAX, PARA) / 2) * roomGenAxis.getOppDirOffset());
    newPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), roomGenAxis.getAxisMod(ROOM_SIZE_MAX, PERP) * roomGenAxis.getOppDirOffset());
    roomGenRect[loop].setCorner(roomGenAxis.getOppDir(), newPoint);
    newPoint = newDoorPoint;
    newPoint.slidePointViaAxis(roomGenAxis.dim, (roomGenAxis.getAxisMod(ROOM_SIZE_MAX, PARA) / 2) * roomGenAxis.getDirOffset());
    newPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), (ROOM_SIZE_MIN * roomGenAxis.getOppDirOffset()));
    roomGenRect[loop].setCorner(roomGenAxis.dir, newPoint);
    
    loop = (int)getFacingWall(roomGenAxis.getPerpReg());//opposite reg from perp reg attatched by dir from newDoorWall
    newPoint = newDoorPoint;
    newPoint.slidePointViaAxis(roomGenAxis.dim, ((roomGenAxis.getAxisMod(ROOM_SIZE_MAX, PARA) / 2) * roomGenAxis.getOppDirOffset()));
    newPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), roomGenAxis.getAxisMod(ROOM_SIZE_MAX, PERP) * roomGenAxis.getOppDirOffset());
    roomGenRect[loop].setCorner(roomGenAxis.getOppDir(), newPoint);
    newPoint = newDoorPoint;
    newPoint.slidePointViaAxis(roomGenAxis.dim, ((ROOM_SIZE_MIN / 2) * roomGenAxis.getOppDirOffset()));
    //perpdim doesn't need to be changed
    roomGenRect[loop].setCorner(roomGenAxis.dir, newPoint);

    //any reg rects that are beyond level bounds, set back within level bounds
    for(loop = 0; loop < NUM_ROOM_WALLS; loop++)//check each region rect
    {
        if(loop == (int)newDoorWall)//don't check the backward into newDoor
            continue;
        
        rangeRectAxis.setAxisFromWall((objReg)loop);
        
        //any reg rects that are beyond level bounds, set back within level bounds
        if(!_theGame->getLevelBounds().getWallRange(getClockWall((objReg)loop)).doesContain(roomGenRect[loop].getWallLocPoint((objReg)loop)))
            roomGenRect[loop].setWallLoc((objReg)loop, _theGame->getLevelBounds().getWallLocPoint((objReg)loop) - rangeRectAxis.getDirOffset());
    }
    
    //check each room to see if it intersects with any of the three region rects. If it does, we pull that rect inward, to ensure the new room won't intersect any current rooms
    for(roomListIter = _levelRooms.begin(); roomListIter != _levelRooms.end(); roomListIter++)
    {
        for(loop = 0; loop < NUM_ROOM_WALLS; loop++)//clear the bool array
            roomIntersects[loop] = false;
        regToSlide[0] = REG_NULL;
        regToSlide[1] = REG_NULL;
        
        for(loop = 0; loop < NUM_ROOM_WALLS; loop++)//check each region rect
        {
            if(loop == (int)newDoorWall)//don't check the backward into newDoor
                continue;
            
            //any reg rects that this room intersects, set true
            if(roomGenRect[loop].doesRectContainWall(*(*roomListIter)->getRect(), getFacingWall((objReg)loop)))
            {
                regToSlide[0] = (objReg)loop;//if only one reg intersects, we'll catch it here
                roomIntersects[loop] = true;//if multiple reg's intersect, we decide below, and overwrite the previous line
            }
        }
        
        //if multiple reg rects intersect the room, we determine the one reg rect we'll adjust--facing wall will be selected most often, so if the corner is intersected, we go with the non-facing for balance
        if(roomIntersects[(int)getFacingWall(newDoorWall)] && roomIntersects[(int)getClockWall(newDoorWall)] && roomIntersects[(int)getCountclockWall(newDoorWall)])//if the room intersects all 3 reg rects...
            regToSlide[0] = getFacingWall(newDoorWall);//we only adjust oppositeWall
        else if(roomIntersects[(int)getFacingWall(newDoorWall)] && roomIntersects[(int)getClockWall(newDoorWall)])//if the room intersects a corner...
        {
            regToSlide[0] = getClockWall(newDoorWall);
            regToSlide[1] = getFacingWall(newDoorWall);
        }
        else if(roomIntersects[(int)getFacingWall(newDoorWall)] && roomIntersects[(int)getCountclockWall(newDoorWall)])//if the room intersects the other corner...
        {
            regToSlide[0] = getCountclockWall(newDoorWall);
            regToSlide[1] = getFacingWall(newDoorWall);
            
        }
        
        //move rect wall inward
        if(regToSlide[0] != REG_NULL)
        {
            rangeRectAxis.setAxisFromWall(regToSlide[0]);
            if(!roomGenRect[(int)regToSlide[0]].setWallLoc(regToSlide[0], (*roomListIter)->getRect()->getWallLocPoint(getFacingWall(regToSlide[0])) - rangeRectAxis.getDirOffset()))//try the wall move and if it fails (cuz we just tried to move the rect wall past the other rect wall)...
            {
                if(regToSlide[1] != REG_NULL)//we have the other reg to use
                {
                    if(!roomGenRect[(int)regToSlide[1]].setWallLoc(regToSlide[1],
                    (*roomListIter)->getRect()->getWallLocPoint(getFacingWall(regToSlide[1])) - rangeRectAxis.getDirOffset()))//try the other reg. If that doesn't work...
                    {
                        //abort room gen--destroy this room and the previous one if this one wasn't a hallway, destroy the door that leads to that hallway
                        abortRoomGenPath(inRoom);
                        return false;
                    }
                }
                else//if we don't havw the other reg to use...
                {
                    //abort room gen--destroy this room and the previous one if this one wasn't a hallway, destroy the door that leads to that hallway
                    abortRoomGenPath(inRoom);
                    return false;
                }
            }
        }
    }
    
    
    //now that we know our max's, set the random ranges accordingly
    for(loop = 0; loop < NUM_ROOM_WALLS; loop++)
    {
        //don't add the unused direction
        if(loop == (int)newDoorWall)
            continue;
        
        loopReg = (objReg)loop;
        newRoom->getWallGenRanges()[loop].setRangeMin(roomGenRect[loop].getWallLocPoint(getFacingWall(loopReg)));//inner wall
        newRoom->getWallGenRanges()[loop].setRangeMax(roomGenRect[loop].getWallLocPoint(loopReg));//outer wall
        newRoom->getWallGenRanges()[loop].setRandType(RAND_DUNGEON);
        
        _theRandHand->addRandomRange(newRoom->getWallGenRanges()[loop]);
    }
    
    return true;
}

bool CSDungeonLevel::createNewRoom(CSDungObj *inDoor, CSRoom *newRoom, int* newNumDoorQty)
{
    bool            connectToRoom = false;
    int             loop, newRandNum, connectToRoomReturnCode;
    objReg          newDoorWall = REG_NULL;
    CSAxis          roomGenAxis;
    CSPoint         newPoint, newDoorPoint;
    CSRoom          *inRoom = inDoor->getOwner();
    CSRandomRange   hallLengthGen(RAND_DUNGEON, ROOM_SIZE_MIN, HALL_LENGTH_MAX);
    
    list<CSRoom *>::iterator            roomListIter;
    list<CSDungObj *>::iterator         objListIter;
    vector<CSRandomRange*>::iterator    wallLocGenIter;

    /*Setup*/
    
    //add the range to the randHand
    _theRandHand->addRandomRange(hallLengthGen);
    
    //only make a new room if newRoom doesn't have a room to connect to, as determined in createRoomGenRanges
    if(newRoom->getRoomToConnect() != nullptr)
    {
        if(newRoom->isHall())
            connectToRoom = true;
        else
        {
            inRoom->setRoomToConnect(newRoom->getRoomToConnect());
            inRoom->connectToRoom();
            return false;
        }
    }
    
    //create a new location that will be the door in our new room, adjacent to the door we found in inRoom, then connect it to inRoom -- this point is continually referenced in room generation
    if(inDoor == nullptr)
        return false;
    
    newDoorWall = getFacingWall(inDoor->getRegion());
    roomGenAxis.setAxisFromWall(newDoorWall);//set the dim to HORIZ or VERT and the dir to UP_LEFT or DOWN_RIGHT
    newDoorPoint = (*inDoor->getLoc());
    newDoorPoint.slidePointViaAxis(roomGenAxis.getPerpAxis(), roomGenAxis.getOppDirOffset());//offset the appropriate axis 1 away, using roomGenAxis (e.g.: x-- or y-- OR x++ or y++)
    newRoom->createObject(OBJ_DOOR, newDoorWall, newDoorPoint, nullptr, inDoor);//create the new door, around which the entire room is to be based, and add it to newRoom
    
    /*Make the room*/
    //dynamically generate a new rect, and create a room, from newDoor
    newPoint = newDoorPoint;//start with newDoor's loc
    
    if(newRoom->isHall())
        newPoint.slidePointViaAxis(roomGenAxis.dim, (HALL_SIZE / 2) * roomGenAxis.getDirOffset());//then slide along newDoorWall to create the room's nearest point (to the right of newDoor, from the perspective of someone walking into newRoom)
    else//non-hall room
    {
        if(_theGame->getBreakState())
            newRandNum = 0;//leave break point for debug purposes.
        
        //set perpendicular wall (to the right of newDoor, from the perspective of someone walking into newRoom) up to half room size away from newDoor
        newRandNum = _theRandHand->getNumber(&newRoom->getWallGenRanges()[(int)roomGenAxis.getPerpReg()]);
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
        newRandNum = _theRandHand->getNumber(&newRoom->getWallGenRanges()[(int)getFacingWall(newDoorWall)]);
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), newRandNum);
        
        //set opposite, perpendicular wall (to the left of newDoor and across the room, from the perspective of someone walking into newRoom) to half room size away from door, the other way
        newRandNum = _theRandHand->getNumber(&newRoom->getWallGenRanges()[(int)getFacingWall(roomGenAxis.getPerpReg())]);
        newPoint.setAxisPoint(roomGenAxis.dim, newRandNum);
    }
    newRoom->getRect()->setCorner(roomGenAxis.getOppDir(), newPoint);//create the further away corner of the new room
    
    
    /*Make a door/some doors*/
    //add a new door or three to newRoom, for the next iteration of createNewRoom
    if(newRoom->isHall())
        newRoom->createNewDoor(REG_NULL);
    else
    {
        newRandNum = _theDoorHand->getNewDoorQuantity(_maxNumDoors - (int)_levelRooms.size());//gets number of doors to create, based on how closed to the max we are
        newNumDoorQty[newRandNum]++;
        for(loop = 0; loop < newRandNum; loop++)
            newRoom->createNewDoor(REG_NULL);
    }
        
    if(connectToRoom)//if we determined we need to connect this hallway to a previously existing room
    {
        connectToRoomReturnCode = newRoom->connectToRoom();
        if(connectToRoomReturnCode == RETURN_CODE_FALSE)//do so now!
        {
            //if inRoom is too close to roomToConnect to make newRoom (due to rect's not being able to be smaller than 1 tile wide), delete newRoom and connect inRoom instead!
            newRoom->getConnectedDoor()->setConnect(nullptr);
            inRoom->setRoomToConnect(newRoom->getRoomToConnect());
            inDoor->setConnect(nullptr);
            if(inRoom->connectToRoom() == RETURN_CODE_ABORT_GEN)
                abortRoomGenPath(inRoom);
            return false;
        }
        else if(connectToRoomReturnCode == RETURN_CODE_ABORT_GEN)
        {
            abortRoomGenPath(newRoom);
            return false;
        }
    }
    
    //final steps!
    newRoom->setRoomNum((int)_levelRooms.size());
    updateDungeonBounds(newRoom);
    
    return true;
}

void CSDungeonLevel::abortRoomGenPath(CSRoom *inRoom)
{
    bool        isHall;
    CSRoom      *prevRoom;
    CSDungObj   *pathDoor = nullptr;
    
    list<CSDungObj *>::iterator   objectIter;
    
    //if connectToRoom sent in newRoom, the assumed logic must be flipped
    isHall = inRoom->isHall();
    if(inRoom->getRoomNum() == BAD_DATA)
        isHall = !isHall;
    
    if(!inRoom->isHall())//newRoom is a hallway, and we only have to delete pathDoor
        pathDoor = inRoom->getUnconnectedDoor();
    else//newRoom is a non-hallway, and we have to delete the inRoom hallway and pathDoor
    {
        prevRoom = inRoom->getConnectedDoor()->getConnect()->getOwner();
        
        //find the door on the other side of the inRoom hallway
        for(objectIter = prevRoom->getObjects()->begin(); objectIter != prevRoom->getObjects()->end(); objectIter++)
        {
            if((*objectIter)->getType() != OBJ_DOOR)
                continue;
            
            if((*objectIter) == inRoom->getConnectedDoor()->getConnect())//if this door is the one that connects back to inRoom
            {
                pathDoor = *objectIter;
                break;
            }
        }
        
        if(inRoom->getRoomNum() != BAD_DATA)//if connectToRoom sent in newRoom, we don't want to delete it here
            deleteRoom(inRoom);
    }
    
    pathDoor->getOwner()->deleteObject(pathDoor);
    
    //createDungeon() will handle the deletion of newRoom
}

void CSDungeonLevel::replaceDoor(void)
{
    CSRoom      *newestRoom;
    CSDungObj   *doorToDelete;
    
    newestRoom = _levelRooms.back();
    doorToDelete = newestRoom->getUnconnectedDoor();
    
    if(doorToDelete == nullptr)//if every door is connected, we don't want to delete any of them
        return;
    
    //out with the old, in with the new!
    newestRoom->deleteObject(doorToDelete);
    newestRoom->createNewDoor(REG_NULL);
}

void CSDungeonLevel::createOuterDoor()
{
    int     loop, closestDistToCenter = _theGame->getLevelBounds().botRight.x * _theGame->getLevelBounds().botRight.y;
    objReg  closestRegToCenter = REG_WALL_LEFT;
    CSAxis  loopAxis;
    CSPoint levelCenter(_theGame->getLevelBounds().botRight.x / 2, _theGame->getLevelBounds().botRight.y / 2);
    CSLine  centerToEdge;
    
    for(loop = REG_WALL_LEFT; loop <= REG_WALL_BOT; loop++)
    {
        loopAxis.setAxisFromWall((objReg)loop);
        centerToEdge.setLine(levelCenter.getAxisPoint(loopAxis.dim), _dungeonBounds.getWallLocPoint((objReg)loop));
        
        if(centerToEdge.getSize() < closestDistToCenter)
        {
            closestDistToCenter = centerToEdge.getSize();
            closestRegToCenter = (objReg)loop;
        }
    }
    
    _outerRooms[closestRegToCenter]->createNewDoor(closestRegToCenter);
}

void CSDungeonLevel::deleteRoom(CSRoom *inRoom)
{
    int         lowestRoomNum = (int)_levelRooms.size();
    CSRoom      *roomToDelete = nullptr, *doorToDeleteOwner = nullptr, *lastRoom = nullptr;
    CSDungObj   *doorToDelete = nullptr;
    
    list<CSRoom *>::iterator    roomIter;
    list<CSDungObj *>::iterator objectIter;
    
    //if no room was sent in, assume we want to delete the most recently made room
    if(inRoom == nullptr)
    {
        if(_levelRooms.size() == 0)//don't try to delete last room if there's no rooms to delete
            return;
        
        //get the last room in _levelRooms
        roomIter = _levelRooms.end();
        roomIter--;
        roomToDelete = *roomIter;
        
        //if this room connects back into the dungeon, we want to delete the door that does this connect-back, since that door will never spawn a new room
        if(roomToDelete->getUnconnectedDoor() == nullptr)
        {
            for(objectIter = roomToDelete->getObjects()->begin(); objectIter != roomToDelete->getObjects()->end(); objectIter++)
                if((*objectIter)->getType() == OBJ_DOOR && (*objectIter)->getConnect()->getOwner()->getRoomNum() < lowestRoomNum)
                {
                    lowestRoomNum = (*objectIter)->getConnect()->getOwner()->getRoomNum();//go through all connected doors and find the earliest connected room
                    doorToDeleteOwner = (*objectIter)->getConnect()->getOwner();
                    doorToDelete = (*objectIter)->getConnect();
                }
            
            if(doorToDelete != nullptr)
                doorToDeleteOwner->deleteObject(doorToDelete);
        }
        
        if(_levelRooms.size() > 0)//if we still have some rooms, let's center the game window on the now-last room
        {
            lastRoom = _levelRooms.back();//now that there's fewer rooms in _levelRooms, we need to get the last room in _levelRooms again
            _theGame->centerGameWindow(lastRoom->getRect()->getCenterPoint());
        }
    }
    else
        roomToDelete = inRoom;
    
    _levelRooms.remove(roomToDelete);
    roomToDelete->deleteRoom();
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

void CSDungeonLevel::createStairs()
{
    CSRandomRange   orientationSelector(RAND_DUNGEON, AXIS_HORIZ, AXIS_VERT),
                    sideSelector(RAND_DUNGEON, DIR_UP_LEFT, DIR_DOWN_RIGHT);
    
    _theRandHand->addRandomRange(orientationSelector);
    _theRandHand->addRandomRange(sideSelector);
    
    CSAxis          stairsOrientation((axis)_theRandHand->getNumber(&orientationSelector), (direction)_theRandHand->getNumber(&sideSelector));
    
    _outerRooms[stairsOrientation.getReg()]->createNewObject(OBJ_STAIRS_UP);
    _outerRooms[getFacingWall(stairsOrientation.getReg())]->createNewObject(OBJ_STAIRS_DOWN);
}

void CSDungeonLevel::createTreasure()
{
    int             loop, subLoop, subLoopTotal;
    vector<int>     oddsVect;
    CSRandomList    numNewTreasure(RAND_DUNGEON);
    
    list<CSRoom *>::iterator    listIter;
   
    for(loop = 1; loop < NUM_ROOM_WALLS; loop++)
    {
        subLoopTotal = NO_TREASURE_CHANCES / (pow(loop, 2));//30, 9, 4
        for(subLoop = 0; subLoop < subLoopTotal; subLoop++)
            oddsVect.push_back(loop - 1);
    }
    numNewTreasure.addListToList(&oddsVect);
    
    for(listIter = _levelRooms.begin(); listIter != _levelRooms.end(); listIter++)
    {
        if((*listIter)->isHall())
            continue;
        
        subLoopTotal = _theRandHand->getNumber(&numNewTreasure);
        
        for(subLoop = 0; subLoop < subLoopTotal; subLoop++)
            (*listIter)->createNewObject(OBJ_TREASURE);
    }
}

#pragma mark -
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
                printf("%c", EMPTY_CHAR);
            
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

