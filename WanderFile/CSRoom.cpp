//
//  CSRoom.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/2/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSRoom.hpp"
#include "CSRandomRange.hpp"
#include "CSDungObj.hpp"

#pragma mark Constructors

CSRoom::CSRoom(CSGameState *incomingGame)
{
    _theGame = incomingGame;
}

CSRoom::CSRoom(CSGameState *incomingGame, CSPoint incomingTopLeft, CSPoint incomingBotRight)
{
    _theGame = incomingGame;
    
    _roomRect.topLeft = incomingTopLeft;
    _roomRect.botRight = incomingBotRight;
}


#pragma mark -
#pragma mark Setters

void CSRoom::setHallState(bool incomingState)
{
    _isHall = incomingState;
}

void CSRoom::setRoomNum(int incomingRoomNum)
{
    char        ones, tens;
    CSPoint     onesLoc, tensLoc;
    CSDungObj   *newRoomNum;
    
    list<CSDungObj *>::iterator  listIter;
    
    _roomNum = incomingRoomNum;
    
    //remove old room number objects
    for(listIter = _objects.begin(); listIter != _objects.end(); listIter++)
        if((*listIter)->getType() == OBJ_TYPE_ROOM_NUM)
        {
            _objects.erase(listIter);
            (*listIter)->deleteObject();
            //delete *listIter;
        }
    
    //inset from top & left walls by 1 tile
    tensLoc.x = _roomRect.topLeft.x + 1;
    tensLoc.y = _roomRect.topLeft.y + 1;
    onesLoc.x = tensLoc.x + 1;
    onesLoc.y = tensLoc.y;
    
    ones = (_roomNum % 10) + '0';
    tens = ((_roomNum / 10) % 10) + '0';//plus ascii offset
    
    newRoomNum = createObject(tens, OBJ_TYPE_ROOM_NUM, REG_WALL_TOP, tensLoc, nullptr);
    createObject(ones, OBJ_TYPE_ROOM_NUM, REG_WALL_TOP, onesLoc, newRoomNum);
}


#pragma mark -
#pragma mark Doers

CSDungObj* CSRoom::createObject(char incomingObjectChar, int incomingObjType, objReg incomingObjReg, CSPoint incomingObjLoc, CSDungObj *incomingCon)
{
    CSDungObj *newObject = new CSDungObj(incomingObjectChar, incomingObjType, incomingObjReg, incomingObjLoc, this);
    
    _objects.push_back(newObject);
    _objects.sort(_objectComparator);
    
    return newObject;
}

void CSRoom::createDoor(objReg incomingDoorWall, CSPoint incomingObjLoc, CSDungObj *incomingCon)
{
    CSDungObj *newObject = new CSDungObj(incomingDoorWall, incomingObjLoc, incomingCon, this);
    
    _objects.push_back(newObject);
    _objects.sort(_objectComparator);
}

void CSRoom::addDoor(CSDungObj *incomingDoor, objReg incomingWall, CSPoint incomingLoc, CSDungObj *incomingCon, CSRoom *incomingOwner)
{
    incomingDoor->setChar(OPEN_DOOR_CHAR);
    incomingDoor->setType(OBJ_TYPE_DOOR);
    incomingDoor->setRegion(incomingWall);
    incomingDoor->setLoc(incomingLoc);
    incomingDoor->setConnectTo(incomingCon);
    incomingDoor->setOwner(incomingOwner);
    
    _objects.push_back(incomingDoor);
    _objects.sort(_objectComparator);
}

/*void CSRoom::removeConnection(CSRoom *incomingRoom)
{
    CSDungObj   *connectedDoor;
    list<CSDungObj *>::iterator listIter;
    
    for(listIter = _objects.begin(); listIter != _objects.end(); listIter++)
    {
        connectedDoor = (*listIter)->getConnect();
        
        if(connectedDoor == nullptr)
            continue;
        
        if((*listIter)->getConnect()->getOwner() == incomingRoom)
            (*listIter)->setConnectTo(nullptr);//listIter = _objects.erase(listIter);//this will delete the door itself, which we don't necessarily want.
    }
    
}*/

void CSRoom::deleteRoom(void)
{
    list<CSDungObj *>::iterator     listIter = _objects.begin();
    
    //erases and deletes all objects, removes the reference back to this CSRoom from all connected CSRoom's
    while(listIter != _objects.end())
    {
        (*listIter)->deleteObject();
        listIter = _objects.erase(listIter);//new iterator properly goes through the list, now with fewer entries
    }
    
    delete this;
}

#pragma mark -

char CSRoom::checkForObject(CSPoint incomingLocation, char incomingObjectChar)
{
    list<CSDungObj *>::iterator   listIter;
    
    for(listIter = _objects.begin(); listIter != _objects.end(); listIter++)
        if((*(*listIter)->getLoc()) == incomingLocation)
            return (*listIter)->getChar();
    
    return incomingObjectChar;//if no object was found at this location, return what we expected to find
}

bool CSRoom::doesRoomAlign(int incomingDimension, CSRoom *incomingRoom)
{
    if(incomingDimension == HORIZ)
        return ((_roomRect.botRight.y > incomingRoom->getRect()->topLeft.y) && (_roomRect.topLeft.y < incomingRoom->getRect()->botRight.y));
    else if(incomingDimension == VERT)
        return ((_roomRect.botRight.x > incomingRoom->getRect()->topLeft.x) && (_roomRect.topLeft.x < incomingRoom->getRect()->botRight.x));
    else
        return false;
}

bool CSRoom::isPointInFreeWall(CSPoint incomingPoint, objReg incomingWall)
{
    CSRange wallRange;
    int     maxWallRange = _roomRect.getWallEndPoint(incomingWall) - 1;
    
    vector<CSRange>             freeWallRanges;
    vector<CSRange>::iterator   vectIter;
    list<CSDungObj*>::iterator  listIter;
    
    wallRange.setRange(_roomRect.getWallStartPoint(incomingWall) + 1, maxWallRange);//set our range up as the entire wall, less corners (which are not free walls)
    
    for(listIter = _objects.begin(); listIter != _objects.end(); listIter++)
    {
        //doors on other walls, or non-doors can be skipped entirely
        if((*listIter)->getType() != OBJ_TYPE_DOOR || (*listIter)->getRegion() != incomingWall)
            continue;
        
        wallRange.max = (*listIter)->getLoc()->getAxisPoint(getWallAxis(incomingWall)) - 1;//set max to just before current door, along wall
        if(wallRange.max >= wallRange.min)//if it makes sense as a range...
            freeWallRanges.push_back(wallRange);//add it
        
        wallRange.setRange((*listIter)->getLoc()->getAxisPoint(getWallAxis(incomingWall)) + 1, _roomRect.getWallEndPoint(incomingWall) - 1);//set max back to just before end point, and min to just after current door
    }
    
    if(wallRange.max >= wallRange.min && wallRange.max <= maxWallRange)//if it makes sense as a range, and doesn't go outside of the original, overall wall range...
        freeWallRanges.push_back(wallRange);//add final range from after last door of loop to just before end point (corner)
    
    //now go through each range and see if the incomingPoint is within any of them
    for(vectIter = freeWallRanges.begin(); vectIter != freeWallRanges.end(); vectIter++)
        if(vectIter->doesContain(incomingPoint.getAxisPoint(getWallAxis(incomingWall))))//only check the a`xis that pertains to this wall
            return true;
           
    return false;
}

bool CSRoom::slideRoom(CSPoint incomingVector)
{
    CSDungObj   *connectedDoor;
    CSRect      oldRoomLoc;
    int         nullVectorAxis = BAD_DATA, axis = BAD_DATA;
    bool        success = true;
    
    vector<CSDungObj *>             slidDoors;
    vector<CSDungObj *>::iterator   vectIter;
    list<CSDungObj *>::iterator     listIter;
    
    if(_theGame->getBreakState())
        oldRoomLoc.calculateArea();
    
    if(!_isHall)//if it's a room, we slide once, then the loop checks if it's okay
    {
        oldRoomLoc = _roomRect;
        _roomRect.slideRect(incomingVector);
    }
    
    //loop through every door's connected door to see if it can be slid
    for(listIter = _objects.begin(); listIter != _objects.end(); listIter++)
    {
        //ignore non-doors and non-connected doors
        if((*listIter)->getType() != OBJ_TYPE_DOOR || (*listIter)->getConnect() == nullptr)
            continue;
        
        connectedDoor = (*listIter)->getConnect();
        axis = getWallAxis((*listIter)->getRegion());
        
        if(!_isHall && (incomingVector.getAxisPoint(getWallAxis((*listIter)->getRegion())) != 0))//if this is a para room-door
            success = (((*listIter)->getLoc()->getAxisPoint(axis) > _roomRect.getWallStartPoint((*listIter)->getRegion())) &&
                       ((*listIter)->getLoc()->getAxisPoint(axis) < _roomRect.getWallEndPoint((*listIter)->getRegion())));//no doors are moving along their wall, so rather than usingthe freeWall system, let's only check if this door is still between the wall's corners, now that the room has moved around it
        else//else, this is a perp or para hall-door, or a perp room-door
        {
            success = connectedDoor->slideObject(incomingVector);//if it let us slide it
            if(success)
                slidDoors.push_back(connectedDoor);//it's been slid, so add it to the list
        }
        
        if(!success)
            break;
    }
    
    //if no doors failed their slide, we can go ahead and slide this room!
    if(success)
    {
        if(_isHall)//non-halls were already slid to test doors
            _roomRect.slideRect(incomingVector);
        else//only non-halls care about this
        {
            //only one axis in the vector could ever be 0
            if(incomingVector.x == 0)
                nullVectorAxis = HORIZ;
            else if(incomingVector.y == 0)
                nullVectorAxis = VERT;
        }
        
        //and all its objects
        for(listIter = _objects.begin(); listIter != _objects.end(); listIter++)
            if(_isHall || (!_isHall && getWallAxis((*listIter)->getRegion()) == nullVectorAxis) || (*listIter)->getConnect() == nullptr)//don't move any doors that're on para slides unless they haven't been connected to anything
                (*listIter)->getLoc()->slidePoint(incomingVector);
    }
    else//but if any doors failed their slide, we undo what we have done
    {
        for(vectIter = slidDoors.begin(); vectIter != slidDoors.end(); vectIter++)//go through the vector of doors that did let us slide them
            (*vectIter)->slideObject(incomingVector * -1);//and undo the slide
        
        if(!_isHall)
            _roomRect = oldRoomLoc;//undo the room's slide
    }
    
    return success;
}

string CSRoom::printRoomRow(CSRange printRange, int rowToPrint)
{
    int     leftPrintBound, rightPrintBound;
    CSPoint tileToPrint;
    string  printString = "";
    bool    printLeftWall, printRightWall;
    
    //deteremine what part of the room is visible within the window, and set bounds accordingly
    if(printRange.min < _roomRect.topLeft.x)
        leftPrintBound = _roomRect.topLeft.x;
    else
        leftPrintBound = printRange.min;
    printLeftWall = printRange.min < _roomRect.topLeft.x;
    
    if(printRange.max > _roomRect.botRight.x)
        rightPrintBound = _roomRect.botRight.x;
    else
        rightPrintBound = printRange.max;
    printRightWall = printRange.max > _roomRect.botRight.x;
    
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

list<CSDungObj*>* CSRoom::getObjects(void)
{
    return &_objects;
}

CSRect* CSRoom::getRect(void)
{
    return &_roomRect;
}



