//
//  CSRoom.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/2/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSLine.hpp"
#include "CSRoom.hpp"
#include "CSRandomRange.hpp"
#include "CSDungObj.hpp"

#pragma mark Constructors

CSRoom::CSRoom(CSGameState *inGame, CSRandomHandler *inRandHand)
{
    _theGame = inGame;
    _theRandHand = inRandHand;
    _roomNumDigits = 0;
    _roomToConnect = nullptr;
    _roomToConnectDist = BAD_DATA;
    _roomNum = BAD_DATA;
}

CSRoom::CSRoom(CSGameState *inGame, CSRandomHandler *inRandHand, CSPoint inTopLeft, CSPoint inBotRight)
{
    _theGame = inGame;
    _theRandHand = inRandHand;
    _roomNumDigits = 0;
    _roomToConnect = nullptr;
    _roomToConnectDist = BAD_DATA;
    _roomNum = BAD_DATA;
    
    _roomRect.topLeft = inTopLeft;
    _roomRect.botRight = inBotRight;
}


#pragma mark -
#pragma mark Setters

void CSRoom::setHallState(bool inState)
{
    _isHall = inState;
}

void CSRoom::setRoomNum(int inRoomNum)
{
    _roomNum = inRoomNum;
    updateRoomNum(_roomNumDigits);
}

void CSRoom::setRoomToConnect(CSRoom *inRoom)
{
    _roomToConnect = inRoom;
}

void CSRoom::setRoomToConnectDist(int inDist)
{
    _roomToConnectDist = inDist;
}

#pragma mark -
#pragma mark Doers - Create/Delete Functions

CSDungObj* CSRoom::createObject(objType inObjType, objReg inObjReg, CSPoint inObjLoc, CSDungObj *inParent, CSDungObj *inCon)
{
    CSDungObj   *newObject = new CSDungObj(inObjType, inObjReg, inObjLoc, inParent, inCon, this);
    
    _objects.push_back(newObject);
    updateObjectNums();
    
    return newObject;
}

void CSRoom::createNewDoor(void)
{
    bool            goodDoorLoc;
    objReg          nextDoorWall = REG_NULL;
    CSAxis          roomGenAxis;
    CSPoint         newPoint;
    CSRandomRange   roomSideGen(RAND_ROOM, REG_WALL_LEFT, REG_WALL_BOT);
    
    list<CSDungObj *>::iterator objListIter;
    
    _theRandHand->addRandomRange(roomSideGen);//add the range to the randHand
    
    if(_isHall)//to the opposite wall for a hallway
    {
        roomGenAxis.setAxisFromWall(getConnectedDoor()->getRegion());//set the dim to HORIZ or VERT and the dir to UP_LEFT or DOWN_RIGHT
        nextDoorWall = getFacingWall(getConnectedDoor()->getRegion());
        
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), _roomRect.getWallLocPoint(nextDoorWall));
        newPoint.setAxisPoint(roomGenAxis.dim, _roomRect.topLeft.getAxisPoint(roomGenAxis.dim) + (HALL_SIZE / 2));
    }
    else//or to a random wall for a room
    {
        do
        {
            nextDoorWall = (objReg)_theRandHand->getNumber(&roomSideGen);
            goodDoorLoc = true;
            
            //make sure it's a wall that has no door, and that adding a door now won't create problems with very nearby rooms, in the next iteration -- in the future, same door walls should be okay, within reason, so we need to create some check for distance minimum between same door walls.
            for(objListIter = _objects.begin(); objListIter != _objects.end(); objListIter++)
                if(nextDoorWall == (*objListIter)->getRegion())
                    goodDoorLoc = false;
        }
        while(!goodDoorLoc);
        
        //dynamically set the door loc to be along the chosen wall at a random point
        CSRandomRange   doorLocGen(RAND_ROOM, _roomRect.getWallStartPoint(nextDoorWall) + 1, _roomRect.getWallEndPoint(nextDoorWall) - 1);//offsets keep doors from appearing in corners
        _theRandHand->addRandomRange(doorLocGen);
        
        //dynamically set the door's loc in the wall
        roomGenAxis.setAxisFromWall(nextDoorWall);
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), _roomRect.getWallLocPoint(nextDoorWall));
        newPoint.setAxisPoint(roomGenAxis.dim, _theRandHand->getNumber(&doorLocGen));
    }
    
    createObject(OBJ_DOOR, nextDoorWall, newPoint, nullptr, nullptr);//make the next door for the next room, because we don't have the door yet
    
    _theRandHand->clearRandomItems(RAND_ROOM);
}

void CSRoom::deleteRoom(void)
{
    list<CSDungObj *>::iterator     objectIter = _objects.begin();
    
    //erases and deletes all objects, removes the reference back to this CSRoom from all connected CSRoom's
    while(objectIter != _objects.end())
    {
        (*objectIter)->deleteObject();
        objectIter = _objects.erase(objectIter);//new iterator properly goes through the list, now with fewer entries
    }
    
    delete this;
}

void CSRoom::deleteObject(int inObjNum)
{
    list<CSDungObj *>::iterator objectIter;
    
    //goes through all objects looking for the incoming object number and deletes it
    for(objectIter = _objects.begin(); objectIter != _objects.end(); objectIter++)
    {
        if((*objectIter)->getNum() == inObjNum)
        {
            (*objectIter)->deleteObject();
            _objects.erase(objectIter);
            break;
        }
    }
    
    updateObjectNums();//then re-numbers the remaining objects
}

void CSRoom::deleteObject(CSDungObj *inObj)
{
    list<CSDungObj *>::iterator objectIter;
    
    //goes through all objects looking for the incoming object and deletes it
    for(objectIter = _objects.begin(); objectIter != _objects.end(); objectIter++)
    {
        if((*objectIter) == inObj)
        {
            (*objectIter)->deleteObject();
            _objects.erase(objectIter);
            break;
        }
    }
    
    updateObjectNums();//then re-numbers the remaining objects
}

#pragma mark -
#pragma mark Doers - Check/Edit Functions

char CSRoom::checkForObject(CSPoint inLocation, char inObjectChar)
{
    list<CSDungObj *>::iterator   listIter;
    
    for(listIter = _objects.begin(); listIter != _objects.end(); listIter++)
        if((*(*listIter)->getLoc()) == inLocation)
            return (*listIter)->getChar();
    
    return inObjectChar;//if no object was found at this location, return what we expected to find
}

int CSRoom::connectToRoom(void)
{
    objReg      wallToConnect, connectingWall;
    CSPoint     newDoorPoint, testPoint;
    CSLine      sharedOverlap;
    CSAxis      hallwayAxis;
    CSRoom      *connectedRoom;
    CSDungObj   *unconnectedDoor, *doorToIgnore;
    
    unconnectedDoor = getUnconnectedDoor();
    wallToConnect = unconnectedDoor->getRegion();
    connectingWall = getFacingWall(wallToConnect);
    hallwayAxis.setAxisFromWall(wallToConnect);
    hallwayAxis.setAxisFromWall(hallwayAxis.getPerpReg());
    newDoorPoint = *unconnectedDoor->getLoc();
    
    if(_isHall)
    {
        connectedRoom = getConnectedDoor()->getConnect()->getOwner();
        doorToIgnore = connectedRoom->getDoorConnectedToRoom(this);
    }
    else//if we're a non-hallway, we want to slide along our own wallToConnect, rather than the room leading into us, the way we would with a hallway
    {
        connectedRoom = this;
        doorToIgnore = getUnconnectedDoor();
    }
    
    newDoorPoint.setAxisPoint(hallwayAxis.dim, _roomToConnect->getRect()->getWallLocPoint(connectingWall));//slide newDoorPoint into roomToConnect, where the new door might be created
    newDoorPoint.slidePointViaAxis(hallwayAxis.dim, hallwayAxis.getOppDirOffset());//set newDoorPoint to be just outside of roomToConnect's facing wall
    unconnectedDoor->setLoc(newDoorPoint);
    
    //set our open side to just before _roomToConnect (get wall that has unconnected door, get facing wall, connect to that wall). If doing so would have flipped the room inside out...
    if(!getRect()->setWallLoc(wallToConnect, newDoorPoint.getAxisPoint(hallwayAxis.dim)))
        return RETURN_CODE_FALSE;//delete this hallway and proceed with the room connected to this hallway
    
    testPoint = newDoorPoint;
    
    //create a range from the largest topLeft to the smallest botRight, so we only slide the room to places they can both connect
    sharedOverlap.setStart(connectedRoom->getRect()->topLeft.getAxisPoint(hallwayAxis.getPerpAxis()));
    if(_roomToConnect->getRect()->topLeft.getAxisPoint(hallwayAxis.getPerpAxis()) > sharedOverlap.getStart())
        sharedOverlap.setStart(_roomToConnect->getRect()->topLeft.getAxisPoint(hallwayAxis.getPerpAxis()));
    sharedOverlap.setEnd(connectedRoom->getRect()->botRight.getAxisPoint(hallwayAxis.getPerpAxis()));
    if(_roomToConnect->getRect()->botRight.getAxisPoint(hallwayAxis.getPerpAxis()) < sharedOverlap.getEnd())
        sharedOverlap.setEnd(_roomToConnect->getRect()->botRight.getAxisPoint(hallwayAxis.getPerpAxis()));
    
    //if the point in the roomToConnect doesn't work, slide along the walls until you find a point that does, testing both connected rooms at each slide point
    while(!_roomToConnect->isWallPointFree(testPoint, connectingWall, nullptr) || !connectedRoom->isWallPointFree(testPoint, wallToConnect, doorToIgnore))
    {
        testPoint.slidePointViaAxis(hallwayAxis.getPerpAxis(), 1);//slide one to the botRight
        if(testPoint.getAxisPoint(hallwayAxis.getPerpAxis()) > sharedOverlap.getEnd())//if we're past the bottom/left-most point where the rooms overlap
            testPoint.setAxisPoint(hallwayAxis.getPerpAxis(), sharedOverlap.getStart());//slide all the way to the top/right-most point where the rooms overlap
        if(testPoint == newDoorPoint)//if we've gone a whole loop through the wall and we still can't connect
            return RETURN_CODE_ABORT_GEN;
    }
    if(slideRoom(testPoint - newDoorPoint))
        newDoorPoint = testPoint;
    
    newDoorPoint.setAxisPoint(hallwayAxis.dim, _roomToConnect->getRect()->getWallLocPoint(connectingWall));//slide newDoorPoint into roomToConnect, where the new door might be created
    _roomToConnect->createObject(OBJ_DOOR, connectingWall, newDoorPoint, nullptr, unconnectedDoor);//now we've found a good spot for it, create a new door in _roomToConnect using newDoorPoint to match our unconnected door, and connect them to each other
    
    //we no longer have a room to connect to, so let's reset our variables
    _roomToConnect = nullptr;
    _roomToConnectDist = BAD_DATA;
    return RETURN_CODE_TRUE;
}

void CSRoom::updateRoomNum(int inNumDigits)
{
    bool            vertHall = false;
    int             loop, powerResult;
    char            newDigit;
    CSPoint         newDigitLoc;
    CSDungObj       *newRoomNum, *prevRoomNum = nullptr;
    
    list<CSDungObj *>::iterator  listIter = _objects.begin();
    
    if(inNumDigits == _roomNumDigits)
        return;
    
    //remove old room number objects
    while(listIter != _objects.end())
    {
        if((*listIter)->getType() == OBJ_ROOM_NUM)
        {
            (*listIter)->deleteObject();
            listIter = _objects.erase(listIter);
        }
        else
            listIter++;
    }

    if(_isHall && _roomRect.getHeight() > _roomRect.getWidth())//the real way to determine this is to check whether top & bot walls have doors, which could easily be set up during room creation, if it matters that much
        vertHall = true;
    
    //create room nums based on new number of digits
    for(loop = 1; loop <= inNumDigits; loop++)
    {
        if(vertHall)
            newDigitLoc.setPoints(_roomRect.topLeft.x + 1, (_roomRect.topLeft.y + inNumDigits) - loop + 1);//inset from top & left walls by 1 tile
        else
            newDigitLoc.setPoints((_roomRect.topLeft.x + inNumDigits) - loop + 1, _roomRect.topLeft.y + 1);//inset from top & left walls by 1 tile
        
        powerResult = pow(10, loop - 1);
        newDigit = ((_roomNum / powerResult) % 10) + '0';//plus ascii offset
        
        newRoomNum = createObject(OBJ_ROOM_NUM, REG_CORNER_TOP_LEFT, newDigitLoc, prevRoomNum, nullptr);//creates one's place and connect it to ten's place
        newRoomNum->setChar(newDigit);
        
        prevRoomNum = newRoomNum;
    }

}

void CSRoom::updateObjectNums(void)
{
    int count = 0;
    
    list<CSDungObj*>::iterator  objectIter;
    
    _objects.sort(_objectComparator);
    
    for(objectIter = _objects.begin(); objectIter != _objects.end(); objectIter++)
    {
        (*objectIter)->setNum(count);
        count++;
    }
}

CSDungObj* CSRoom::getUnconnectedDoor(void)
{
    list<CSDungObj *>::iterator objListIter;
    
    for(objListIter = _objects.begin(); objListIter != _objects.end(); objListIter++)
        if((*objListIter)->getType() == OBJ_DOOR && (*objListIter)->getConnect() == nullptr)//this is a door & it should connect to our new room
            return *objListIter;
    
    return nullptr;
}

CSDungObj* CSRoom::getDoorConnectedToRoom(CSRoom *inRoom)
{
    list<CSDungObj *>::iterator objListIter;
    
    for(objListIter = _objects.begin(); objListIter != _objects.end(); objListIter++)
        if((*objListIter)->getType() == OBJ_DOOR && (*objListIter)->getConnect()->getOwner() == inRoom)
            return *objListIter;
    
    return nullptr;
}

CSDungObj* CSRoom::getConnectedDoor(void)
{
    list<CSDungObj *>::iterator objListIter;
    
    for(objListIter = _objects.begin(); objListIter != _objects.end(); objListIter++)
        if((*objListIter)->getType() == OBJ_DOOR && ((*objListIter)->getConnect() != nullptr))//this is a door & it should connect to our new room
            return (*objListIter);
    
    return nullptr;
}

/*bool CSRoom::isPointInFreeWall(CSPoint inPoint, objReg inWall)
{
    CSAxis  wallAxis;
    CSRange roomWallRange(_roomRect.getWallStartPoint(inWall) + 1, _roomRect.getWallEndPoint(inWall) - 1),//set our range up as the entire wall, less corners (which are not free walls)
            wallRangeSegment;
    
    vector<CSRange>             freeWallRanges;
    vector<CSRange>::iterator   vectIter;
    list<CSDungObj*>::iterator  listIter;
    
    wallAxis.setAxisFromWall(inWall);
    wallRangeSegment = roomWallRange;
    
    for(listIter = _objects.begin(); listIter != _objects.end(); listIter++)
    {
        //doors on other walls, or non-doors can be skipped entirely
        if((*listIter)->getType() != OBJ_DOOR || (*listIter)->getRegion() != inWall)
            continue;
        
        wallRangeSegment.setMax((*listIter)->getLoc()->getAxisPoint(wallAxis.dim) - HALL_SIZE);//set max to just before the wall of the hallway connected to current door, along wall
        if(wallRangeSegment.getMin() >= roomWallRange.getMin() && wallRangeSegment.getMax() > roomWallRange.getMin())//if we haven't left the roomWallRange limits...
            freeWallRanges.push_back(wallRangeSegment);//add it to our vector of range segments
        
        wallRangeSegment.setRange((*listIter)->getLoc()->getAxisPoint(wallAxis.dim) + HALL_SIZE, roomWallRange.getMax());//set max back to just before end point, and min to just after the wall of the hallway connected to current door
        if(wallRangeSegment.getMin() >= roomWallRange.getMax() || wallRangeSegment.getMax() >= roomWallRange.getMax())//if we have left the roomWallRange limits...
            continue;//then this wall is likely too small to have any free points
    }
    
    if(wallRangeSegment.getMax() <= roomWallRange.getMax())//if it makes sense as a range, and doesn't go outside of the original, overall wall range...
        freeWallRanges.push_back(wallRangeSegment);//add final range from after last door of loop to just before end point (corner)
    
    //now go through each range and see if the inPoint is within any of them
    for(vectIter = freeWallRanges.begin(); vectIter != freeWallRanges.end(); vectIter++)
        if(vectIter->doesContain(inPoint.getAxisPoint(wallAxis.dim)))//only check the a`xis that pertains to this wall
            return true;
           
    return false;
}*/

bool CSRoom::isWallPointFree(CSPoint inPoint, objReg inWall, CSDungObj *doorToMove)
{
    CSAxis  wallAxis;
    
    list<CSDungObj*>::iterator  objectIter;
    
    wallAxis.setAxisFromWall(inWall);
    
    //if inPoint is on either corner, or is outside the wall range alltogether...
    if(inPoint.getAxisPoint(wallAxis.dim) <= getRect()->getWallStartPoint(inWall) ||
       inPoint.getAxisPoint(wallAxis.dim) >= getRect()->getWallEndPoint(inWall))
        return false;
    
    //check every door on inWall
    for(objectIter = _objects.begin(); objectIter != _objects.end(); objectIter++)
    {
        int objectNum;
        objectNum = (*objectIter)->getNum();
        
        //skip doors on other walls, non-doors, or the door we're moving that's leading to this wall check
        if((*objectIter)->getType() != OBJ_DOOR || (*objectIter)->getRegion() != inWall || (*objectIter) == doorToMove)
            continue;
        
        //if inPoint is where inWall has a connected door
        if(inPoint.getAxisPoint(wallAxis.dim) == (*objectIter)->getLoc()->getAxisPoint(wallAxis.dim))
            return false;
        
        //if inPoint aligns with one of the walls of a connected hallway
        if(inPoint.getAxisPoint(wallAxis.dim) == (*objectIter)->getLoc()->getAxisPoint(wallAxis.dim) - (HALL_SIZE / 2) ||
           inPoint.getAxisPoint(wallAxis.dim) == (*objectIter)->getLoc()->getAxisPoint(wallAxis.dim) + (HALL_SIZE / 2))
           return false;
    }
    
    return true;
}

bool CSRoom::slideRoom(CSPoint inVector)
{
    CSDungObj   *connectedDoor;
    CSRect      oldRoomLoc;
    CSPoint     newLoc;
    CSAxis      slideAxis, roomAxis;
    bool        success = true;
    
    vector<CSDungObj *>             slidDoors;
    vector<CSDungObj *>::iterator   vectIter;
    list<CSDungObj *>::iterator     listIter;
    
    if(_theGame->getBreakState())
        oldRoomLoc.calculateArea();//for debug only
    
    if(!_isHall)//if it's a room, we slide once, then the loop checks if it's okay
    {
        oldRoomLoc = _roomRect;
        _roomRect.slideRect(inVector);
    }
    
    //loop through every door's connected door to see if it can be slid
    for(listIter = _objects.begin(); listIter != _objects.end(); listIter++)
    {
        //ignore non-doors and non-connected doors
        if((*listIter)->getType() != OBJ_DOOR || (*listIter)->getConnect() == nullptr)
            continue;
        
        slideAxis.setAxisFromWall((*listIter)->getRegion());
        
        //if this is a para room-door...
        if(!_isHall && (inVector.getAxisPoint(slideAxis.dim) != 0))
            success = (((*listIter)->getLoc()->getAxisPoint(slideAxis.dim) > _roomRect.getWallStartPoint((*listIter)->getRegion())) &&
                       ((*listIter)->getLoc()->getAxisPoint(slideAxis.dim) < _roomRect.getWallEndPoint((*listIter)->getRegion())));//no doors are moving along their wall, so rather than using the freeWall system, let's only check if this door is still between the wall's corners, now that the room has moved around it
        else//else, this is a perp or para hall-door, or a perp room-door
        {
            //try to slide the door's connected door (in the adjacent room), and if it worked, add it to the slidDoors vector
            connectedDoor = (*listIter)->getConnect();
            
            success = connectedDoor->slideDoor(inVector);
            if(success)
                slidDoors.push_back(connectedDoor);
        }
        
        if(!success)
            break;
    }
    
    //if no doors failed their slide, we can go ahead and slide this room!
    if(success)
    {
        if(_isHall)//non-halls were already slid to test doors
            _roomRect.slideRect(inVector);
        else//only non-halls care about this
        {
            //only one axis in the vector could ever be 0 (otherwise, we wouldn't be sliding at all)
            if(inVector.x == 0)
                slideAxis.dim = AXIS_VERT;
            else if(inVector.y == 0)
                slideAxis.dim = AXIS_HORIZ;
        }
        
        //also slide every object in the room except connected para room-doors
        for(listIter = _objects.begin(); listIter != _objects.end(); listIter++)
        {
            roomAxis.setAxisFromWall((*listIter)->getRegion());
            
            if(((_isHall || (!_isHall && roomAxis.dim != slideAxis.dim) || (*listIter)->getConnect() == nullptr)) &&
               !(*listIter)->getWasMoved())
                (*listIter)->slideObject(inVector);
        }
    }
    else//but if any doors failed their slide, we undo what we have done
    {
        for(vectIter = slidDoors.begin(); vectIter != slidDoors.end(); vectIter++)//go through the vector of doors that did let us slide them
            (*vectIter)->slideDoor(inVector * -1);//and undo the slide
        
        if(!_isHall)
            _roomRect = oldRoomLoc;//undo the room's slide
    }
    
    //reset all objects', and their connects' _wasMoved to false;
    for(listIter = _objects.begin(); listIter != _objects.end(); listIter++)
    {
        (*listIter)->setWasMoved(false);
        if((*listIter)->getConnect() != nullptr)
            (*listIter)->getConnect()->setWasMoved(false);
    }
    
    return success;
}

bool CSRoom::slideWall(objReg inWall, int inVector)
{
    int         newWallLoc = _roomRect.getWallLocPoint(inWall) + inVector, narrowest;
    CSAxis      wallAxis;
    CSRange     wallMovementRange;
    CSRect      newRoomRect;
    
    list<CSDungObj*>::iterator  listIter;
    
    //move the wall this door is on to the same spot
    newRoomRect = _roomRect;
    newRoomRect.setWallLoc(inWall, newWallLoc);
    
    //I'm okay making halls very short, but rooms should never be too narrow
    if(_isHall)
        narrowest = ROOM_SIZE_MIN / 2;
    else
        narrowest = ROOM_SIZE_MIN + 1;
    
    //Before the wall can be slid, check if there are any doors on the tiles between the wall's current and future loc.
    wallAxis.setAxisFromWall(inWall);
    wallMovementRange.setRange(newRoomRect.getWallLocPoint(inWall), _roomRect.getWallLocPoint(inWall));
    
    for(listIter = _objects.begin(); listIter != _objects.end(); listIter++)
    {
        if((*listIter)->getType() != OBJ_DOOR || (*listIter)->getRegion() == inWall)
            continue;
        
        //If there are, cancel the move
        if(wallMovementRange.doesContain((*listIter)->getLoc()->getAxisPoint(wallAxis.getPerpAxis())))
            return false;
    }
    
    //don't let the room be squished to nothing
    if(newRoomRect.getDim(wallAxis.getPerpAxis()) >= narrowest)
        _roomRect.setPoints(newRoomRect.topLeft, newRoomRect.botRight);
    else
        return false;
    
    return true;//if we got this far, we're good
}

#pragma mark -
#pragma mark Doers - Graphics Functions

string CSRoom::printRoomRow(CSRange printRange, int rowToPrint)
{
    int     leftPrintBound, rightPrintBound;
    CSPoint tileToPrint;
    string  printString = "";
    bool    printLeftWall, printRightWall;
    
    //deteremine what part of the room is visible within the window, and set bounds accordingly
    if(printRange.getMin() < _roomRect.topLeft.x)
        leftPrintBound = _roomRect.topLeft.x;
    else
        leftPrintBound = printRange.getMin();
    printLeftWall = printRange.getMin() < _roomRect.topLeft.x;
    
    if(printRange.getMax() > _roomRect.botRight.x)
        rightPrintBound = _roomRect.botRight.x;
    else
        rightPrintBound = printRange.getMax();
    printRightWall = printRange.getMax() > _roomRect.botRight.x;
    
    tileToPrint.y = rowToPrint;
    
    //use rowToPrint to determine which horizontal line of the room to print
    if(rowToPrint == _roomRect.topLeft.y || rowToPrint == _roomRect.botRight.y)//print top or bottom wall
        for(tileToPrint.x = leftPrintBound; tileToPrint.x <= rightPrintBound; tileToPrint.x++)
            printString += checkForObject(tileToPrint, WALL_CHAR);//send in the tile we wish to print and the tile we assume to be there, checkForObject will return the assumed tile or any overridden tile, based on room info, and append it to printString
    else//print the left wall, guts of the room, and right wall
    {
        //print left wall/door
        if(_roomRect.getWidth() > 0 && printLeftWall)
        {
            tileToPrint.x = _roomRect.topLeft.x;
            printString += checkForObject(tileToPrint, WALL_CHAR);
        }
        
        //print floor and/or objects
        for(tileToPrint.x = leftPrintBound + printLeftWall; tileToPrint.x <= rightPrintBound - printRightWall; tileToPrint.x++)//inset by 1 on each side for the walls
            printString += checkForObject(tileToPrint, FLOOR_CHAR);
        
        //print right wall/door
        if(_roomRect.getWidth() > 1 && printRightWall)//left wall and right wall
        {
            tileToPrint.x = _roomRect.botRight.x;
            printString += checkForObject(tileToPrint, WALL_CHAR);
        }
    }
    
    return printString;
}

string CSRoom::printRoomToFile(void)
{
    string  outputString = "Room";
    
    outputString += to_string(_roomNum);
    outputString += ":";
    outputString += to_string(_roomRect.topLeft.x);
    outputString += ",";
    outputString += to_string(_roomRect.topLeft.y);
    outputString += ",";
    outputString += to_string(_roomRect.botRight.x);
    outputString += ",";
    outputString += to_string(_roomRect.botRight.y);
    outputString += "\n";
    
    return outputString;
}


#pragma mark -
#pragma mark Getters

bool CSRoom::isHall(void)
{
    return _isHall;
}

int CSRoom::getRoomNum(void)
{
    return _roomNum;
}

CSRandomRange* CSRoom::getWallGenRanges(void)
{
    return &_wallGenLoc[0];
}

list<CSDungObj*>* CSRoom::getObjects(void)
{
    return &_objects;
}

CSRect* CSRoom::getRect(void)
{
    return &_roomRect;
}

CSRoom* CSRoom::getRoomToConnect(void)
{
    return _roomToConnect;
}

int CSRoom::getRoomToConnectDist(void)
{
    return _roomToConnectDist;
}


