//
//  CSRoom.cpp
//  Random Rooms
//
//  Created by Maxim Boschert-Zielsdorf on 10/2/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include <sstream>
#include <iostream>
#include "CSGameState.hpp"
#include "CSLine.hpp"
#include "CSRandomRange.hpp"
#include "CSRoom.hpp"
#include "CSEntity.hpp"
#include "CSCreature.hpp"

#pragma mark Constructors

CSRoom::CSRoom(CSGameState *inGame)
{
    roomInit(inGame);
}

CSRoom::CSRoom(CSGameState *inGame, CSPoint *inTopLeft, CSPoint *inBotRight)
{
    roomInit(inGame);
    
    _roomRect.topLeft = *inTopLeft;
    _roomRect.botRight = *inBotRight;
}

CSRoom::CSRoom(CSGameState *inGame, CSFileLoader *inFileLoader)//load room from file
{
    int         left = BAD_DATA, top = BAD_DATA, right = BAD_DATA, bot = BAD_DATA, boolAsInt;
    CSEntity   *newEntity;
    
    roomInit(inGame);
    
    inFileLoader->populateDictionary();
   
    //set all the room data
    inFileLoader->getIntValueFromKey(_roomDataKey[0], _roomNum);
    inFileLoader->getIntValueFromKey(_roomDataKey[1], left);
    inFileLoader->getIntValueFromKey(_roomDataKey[2], top);
    inFileLoader->getIntValueFromKey(_roomDataKey[3], right);
    inFileLoader->getIntValueFromKey(_roomDataKey[4], bot);
    _roomRect.setPoints(left, top, right, bot);
    inFileLoader->getIntValueFromKey(_roomDataKey[5], boolAsInt);
    _isHall = (bool)boolAsInt;
    inFileLoader->getIntValueFromKey(_roomDataKey[6], boolAsInt);
    _vertHall = (bool)boolAsInt;
    
    //start on Entities's
    if(inFileLoader->getCompleteState() == FILE_DESCEND_CHAR)
    {
        while(inFileLoader->getCompleteState() != FILE_ASCEND_CHAR)
        {
            inFileLoader->populateDictionary();
            
            newEntity = new CSEntity(_theGame, this, inFileLoader);
            if(newEntity->getOwner() == nullptr)//check to see if the entity discovered that we should be making a creature instead
            {
                delete newEntity;
                newEntity = new CSCreature(_theGame, this, inFileLoader);
            }
            
            _entities.push_back(newEntity);
            updateEntityNums();
        }
    }
}

void CSRoom::roomInit(CSGameState *inGame)
{
    _theGame = inGame;
    _roomNumDigits = 0;
    _roomToConnect = nullptr;
    _roomNum = BAD_DATA;
    _numDoors = 0;
    _vertHall = false;
    
    //set up the keys
    _roomDataKey.push_back("RoomNum");
    _roomDataKey.push_back("Left");
    _roomDataKey.push_back("Top");
    _roomDataKey.push_back("Right");
    _roomDataKey.push_back("Bottom");
    _roomDataKey.push_back("Hall");
    _roomDataKey.push_back("Vert");
}


#pragma mark -
#pragma mark Setters

void CSRoom::setHallState(bool inState)
{
    _isHall = inState;
}

void CSRoom::setVertHallState(bool inState)
{
    _vertHall = inState;
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


#pragma mark -
#pragma mark Doers - Create/Delete Functions

CSEntity* CSRoom::createEntity(entType inEntType, entReg inEntReg, CSPoint *inEntLoc, CSEntity *inParent, CSEntity *inCon)
{
    CSEntity   *newEntity = new CSEntity(_theGame, inEntType, inEntReg, inEntLoc, inParent, inCon, this);
    
    //_entities.push_back(newEntity);//this happens in the entity creation
    updateEntityNums();
    
    return newEntity;
}

void CSRoom::createCoreDoor(entReg inReg, CSPoint *inPoint, CSEntity *inDoor)
{
    createEntity(ENT_DOOR, inReg, inPoint, nullptr, inDoor);
    _numDoors++;
}

void CSRoom::createNewDoor(entReg inReg)
{
    bool            goodDoorLoc;
    entReg          nextDoorWall = REG_NULL;
    CSAxis          roomGenAxis;
    CSPoint         newPoint;
    CSRandomRange   roomSideGen(RAND_ROOM, REG_WALL_LEFT, REG_WALL_BOT);
    
    list<CSEntity *>::iterator entIter;
    
    _theGame->theRandHand.addRandomRange(roomSideGen);//add the range to the randHand
    
    if(_isHall)//to the opposite wall for a hallway
    {
        //if we're a hall, we always send in an entReg--the reg for this new door
        roomGenAxis.setAxisFromWall(inReg);//set the dim to HORIZ or VERT and the dir to UP_LEFT or DOWN_RIGHT
        nextDoorWall = inReg;
        
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), _roomRect.getWallLocPoint(nextDoorWall));
        newPoint.setAxisPoint(roomGenAxis.dim, _roomRect.topLeft.getAxisPoint(roomGenAxis.dim) + (HALL_SIZE / 2));
    }
    else//or to a random wall for a room
    {
        if(inReg == REG_NULL)//we don't care which wall, as long as it's not already with-door
            do
            {
                nextDoorWall = (entReg)_theGame->theRandHand.getNumber(&roomSideGen);
                goodDoorLoc = true;
                
                //make sure it's a wall that has no door, and that adding a door now won't create problems with very nearby rooms, in the next iteration -- in the future, same door walls should be okay, within reason, so we need to create some check for distance minimum between same door walls.
                for(entIter = _entities.begin(); entIter != _entities.end(); entIter++)
                    if(nextDoorWall == (*entIter)->getRegion())
                        goodDoorLoc = false;
            }
            while(!goodDoorLoc);
        else//used for createOuterDoor()
            nextDoorWall = inReg;
        
        //dynamically set the door loc to be along the chosen wall at a random point
        CSRandomRange   doorLocGen(RAND_ROOM, _roomRect.getWallStartPoint(nextDoorWall) + 1, _roomRect.getWallEndPoint(nextDoorWall) - 1);//offsets keep doors from appearing in corners
        _theGame->theRandHand.addRandomRange(doorLocGen);
        
        //dynamically set the door's loc in the wall
        roomGenAxis.setAxisFromWall(nextDoorWall);
        newPoint.setAxisPoint(roomGenAxis.getPerpAxis(), _roomRect.getWallLocPoint(nextDoorWall));
        newPoint.setAxisPoint(roomGenAxis.dim, _theGame->theRandHand.getNumber(&doorLocGen));
    }
    
    _theGame->theDoorHand.addDoor(createEntity(ENT_DOOR, nextDoorWall, &newPoint, nullptr, nullptr));//make the next door for the next room, because we don't have the door yet
    
    _theGame->theRandHand.clearRandomItems(RAND_ROOM);
    _numDoors++;
}

CSEntity* CSRoom::createNewEntity(entType inType)
{
    bool            passable;
    CSPoint         entLoc;
    
    switch(inType)
    {
        case ENT_CREATURE:
            passable = true;
            break;
        case ENT_TREASURE:
            passable = false;
            break;
        default:
            passable = true;
            break;
    }
    
    getGoodRoomPoint(entLoc, passable);
    
    return createEntity(inType, REG_ROOM, &entLoc, nullptr, nullptr);
}

void CSRoom::addEntity( CSEntity *inEnt)//for entities (usually cxreatures) entering the room
{
    if(inEnt->getOwner() != nullptr)
        inEnt->getOwner()->removeEntity(inEnt);
    inEnt->setOwner(this);
    
    _entities.push_back(inEnt);
    
    updateEntityNums();
}

void CSRoom::removeEntity(CSEntity *inEnt)//for entities (usually monsters) leaving the room
{
    _entities.remove(inEnt);
    
    updateEntityNums();
}

void CSRoom::deleteRoom(void)
{
    list<CSEntity *>::iterator     entIter = _entities.begin();
    
    //erases and deletes all entities, removes the reference back to this CSRoom from all connected CSRoom's
    while(entIter != _entities.end())
    {
        //if we're deleting a door, make sure to remove it from theDoorHand, then re-add the now-unconnected door.
        if((*entIter)->getType() == ENT_DOOR)
        {
            _theGame->theDoorHand.removeDoor(*entIter);
            if((*entIter)->getConnect() != nullptr)
                _theGame->theDoorHand.addDoor((*entIter)->getConnect());
        }
        if((*entIter)->getType() != ENT_CREATURE)//creature entities are deleted by DungeonLevel
            (*entIter)->deleteEntity();
        entIter = _entities.erase(entIter);//new iterator properly goes through the list, now with fewer entries
    }
    
    delete this;
}

void CSRoom::deleteEntity(int inEntNum)
{
    list<CSEntity *>::iterator entIter;
    
    //goes through all entities looking for the incoming entities number and has the other deleteEntities method delete it
    for(entIter = _entities.begin(); entIter != _entities.end(); entIter++)
        if((*entIter)->getNum() == inEntNum)
        {
            deleteEntity(*entIter);
            return;
        }
}

void CSRoom::deleteEntity(CSEntity *inEnt)
{
    list<CSEntity *>::iterator entIter;
    
    //goes through all entities looking for the incoming entity and deletes it
    for(entIter = _entities.begin(); entIter != _entities.end(); entIter++)
        if((*entIter) == inEnt)
        {
            if((*entIter)->getType() == ENT_DOOR)
                _theGame->theDoorHand.removeDoor(*entIter);
            
            (*entIter)->deleteEntity();
            _entities.erase(entIter);
            
            updateEntityNums();//then re-numbers the remaining entities
            return;
        }
}


#pragma mark -
#pragma mark Doers

int CSRoom::connectToRoom(void)
{
    bool        goodConnect = false;
    int         loop;
    entReg      wallToConnect, connectingWall;
    CSPoint     newDoorPoint, testPoint, tempPoint;
    CSLine      sharedOverlap;
    CSAxis      hallwayAxis;
    CSRoom      *connectedRoom;
    CSEntity    *unconnectedDoor, *doorToIgnore;
    
    unconnectedDoor = getUnconnectedDoor();
    wallToConnect = unconnectedDoor->getRegion();
    connectingWall = getFacingWall(wallToConnect);
    hallwayAxis.setAxisFromWall(wallToConnect);
    hallwayAxis.setAxisFromWall(hallwayAxis.getPerpReg());
    newDoorPoint = *unconnectedDoor->getLoc();
    
    if(_isHall)
    {
        doorToIgnore = getConnectedDoor()->getConnect();
        connectedRoom = doorToIgnore->getOwner();
    }
    else//if we're a non-hallway, we want to slide along our own wallToConnect, rather than the room leading into us, the way we would with a hallway
    {
        connectedRoom = this;
        doorToIgnore = getUnconnectedDoor();
    }
    
    newDoorPoint.setAxisPoint(hallwayAxis.dim, _roomToConnect->getRect()->getWallLocPoint(connectingWall));//slide newDoorPoint into roomToConnect, where the new door might be created
    newDoorPoint.slidePointViaAxis(hallwayAxis.dim, hallwayAxis.getOppDirOffset());//set newDoorPoint to be just outside of roomToConnect's facing wall
    unconnectedDoor->setLoc(&newDoorPoint);
    
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
    for(loop = 0; loop < sharedOverlap.getSize(); loop++)
    {
        goodConnect = _roomToConnect->isWallPointFree(&testPoint, connectingWall, nullptr) && connectedRoom->isWallPointFree(&testPoint, wallToConnect, doorToIgnore);
        if(goodConnect)
            break;
        
        testPoint.slidePointViaAxis(hallwayAxis.getPerpAxis(), 1);//slide one to the botRight
        if(testPoint.getAxisPoint(hallwayAxis.getPerpAxis()) > sharedOverlap.getEnd())//if we're past the bottom/left-most point where the rooms overlap
            testPoint.setAxisPoint(hallwayAxis.getPerpAxis(), sharedOverlap.getStart());//slide all the way to the top/right-most point where the rooms overlap
    }
    if(!goodConnect)//if we've gone a whole loop through the wall and we still can't connect
        return RETURN_CODE_ABORT_GEN;
    
    //now that we have a good connection point, try the slide. If it works, set our new door point to the successful test point
    tempPoint = testPoint - newDoorPoint;
    if(slideRoom(&tempPoint))
        newDoorPoint = testPoint;
    else
        return RETURN_CODE_ABORT_GEN;
    
    newDoorPoint.setAxisPoint(hallwayAxis.dim, _roomToConnect->getRect()->getWallLocPoint(connectingWall));//slide newDoorPoint into roomToConnect, where the new door might be created
    _roomToConnect->createEntity(ENT_DOOR, connectingWall, &newDoorPoint, nullptr, unconnectedDoor);//now we've found a good spot for it, create a new door in _roomToConnect using newDoorPoint to match our unconnected door, and connect them to each other
    
    //we no longer have a room to connect to, or an unconnected door so let's reset our variables
    _theGame->theDoorHand.removeDoor(unconnectedDoor);
    _roomToConnect = nullptr;
    return RETURN_CODE_TRUE;
}

void CSRoom::updateRoomNum(int inNumDigits)
{
    int             loop, powerResult;
    char            newDigit;
    CSPoint         newDigitLoc;
    CSRect          wallessRect;
    CSEntity        *newRoomNum, *prevRoomNum = nullptr;
    
    list<CSEntity *>::iterator  listIter = _entities.begin();
    
    if(inNumDigits == _roomNumDigits)
        return;
    
    getWallessRect(wallessRect);
    
    //remove old room number entities
    while(listIter != _entities.end())
    {
        if((*listIter)->getType() == ENT_ROOM_NUM)
        {
            (*listIter)->deleteEntity();
            listIter = _entities.erase(listIter);
        }
        else
            listIter++;
    }
    
    //create room nums based on new number of digits
    for(loop = 1; loop <= inNumDigits; loop++)
    {
        if(_vertHall)
            newDigitLoc.setPoints(wallessRect.topLeft.x, (wallessRect.topLeft.y + inNumDigits) - loop);//inset from top & left walls by 1 tile
        else
            newDigitLoc.setPoints((wallessRect.topLeft.x + inNumDigits) - loop, wallessRect.topLeft.y);//inset from top & left walls by 1 tile
        
        powerResult = pow(10, loop - 1);
        newDigit = ((_roomNum / powerResult) % 10) + '0';//plus ascii offset
        
        newRoomNum = createEntity(ENT_ROOM_NUM, REG_CORNER_TOP_LEFT, &newDigitLoc, prevRoomNum, nullptr);//creates one's place and connect it to ten's place
        newRoomNum->setChar(newDigit);
        
        prevRoomNum = newRoomNum;
    }

}

void CSRoom::updateEntityNums(void)
{
    int count = 0;
    
    list<CSEntity*>::iterator  entIter;
    
    _entities.sort(_entComparitor);
    
    for(entIter = _entities.begin(); entIter != _entities.end(); entIter++)
    {
        (*entIter)->setNum(count);
        count++;
    }
}

bool CSRoom::slideRoom(CSPoint *inVector)
{
    CSEntity   *connectedDoor;
    CSRect      oldRoomLoc;
    CSPoint     newLoc, newVect;
    CSAxis      slideAxis, roomAxis;
    bool        success = true;
    
    vector<CSEntity *>             slidDoors;
    vector<CSEntity *>::iterator   vectIter;
    list<CSEntity *>::iterator     listIter;
    
    if(_theGame->getBreakState())
        oldRoomLoc.calculateArea();//for debug only
    
    if(!_isHall)//if it's a room, we slide once, then the loop checks if it's okay
    {
        oldRoomLoc = _roomRect;
        _roomRect.slideRect(inVector);
    }
    
    //loop through every door's connected door to see if it can be slid
    for(listIter = _entities.begin(); listIter != _entities.end(); listIter++)
    {
        //ignore non-doors and non-connected doors
        if((*listIter)->getType() != ENT_DOOR || (*listIter)->getConnect() == nullptr)
            continue;
        
        slideAxis.setAxisFromWall((*listIter)->getRegion());
        
        //if this is a para room-door...
        if(!_isHall && (inVector->getAxisPoint(slideAxis.dim) != 0))
            success = (((*listIter)->getLoc()->getAxisPoint(slideAxis.dim) > _roomRect.getWallStartPoint((*listIter)->getRegion())) &&
                       ((*listIter)->getLoc()->getAxisPoint(slideAxis.dim) < _roomRect.getWallEndPoint((*listIter)->getRegion())));//no doors are moving along their wall, so rather than using the freeWall system, let's only check if this door is still between the wall's corners, now that the room has moved around it
        else//else, this is a perp or para hall-door, or a perp room-door
        {
            //try to slide the door's connected door (in the adjacent room), and if it worked, add it to the slidDoors vector
            connectedDoor = (*listIter)->getConnect();
            
            success = connectedDoor->slideDoor(*inVector);
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
            if(inVector->x == 0)
                slideAxis.dim = AXIS_VERT;
            else if(inVector->y == 0)
                slideAxis.dim = AXIS_HORIZ;
        }
        
        //also slide every entity in the room except connected para room-doors
        for(listIter = _entities.begin(); listIter != _entities.end(); listIter++)
        {
            roomAxis.setAxisFromWall((*listIter)->getRegion());
            
            if(((_isHall || (!_isHall && roomAxis.dim != slideAxis.dim) || (*listIter)->getConnect() == nullptr)) &&
               !(*listIter)->getWasMoved())
                (*listIter)->slideEntity(*inVector);
        }
    }
    else//but if any doors failed their slide, we undo what we have done
    {
        for(vectIter = slidDoors.begin(); vectIter != slidDoors.end(); vectIter++)//go through the vector of doors that did let us slide them
            (*vectIter)->slideDoor(*inVector * -1);//and undo the slide
        
        if(!_isHall)
            _roomRect = oldRoomLoc;//undo the room's slide
    }
    
    //reset all entities', and their connects' _wasMoved to false;
    for(listIter = _entities.begin(); listIter != _entities.end(); listIter++)
    {
        (*listIter)->setWasMoved(false);
        if((*listIter)->getConnect() != nullptr)
            (*listIter)->getConnect()->setWasMoved(false);
    }
    
    return success;
}

bool CSRoom::slideWall(entReg inWall, int inVector)
{
    int         newWallLoc = _roomRect.getWallLocPoint(inWall) + inVector, narrowest;
    CSAxis      wallAxis;
    CSRange     wallMovementRange;
    CSRect      newRoomRect;
    
    list<CSEntity*>::iterator  listIter;
    
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
    
    for(listIter = _entities.begin(); listIter != _entities.end(); listIter++)
    {
        if((*listIter)->getType() != ENT_DOOR || (*listIter)->getRegion() == inWall)
            continue;
        
        //If there are, cancel the move
        if(wallMovementRange.doesContain((*listIter)->getLoc()->getAxisPoint(wallAxis.getPerpAxis())))
            return false;
    }
    
    //don't let the room be squished to nothing
    if(newRoomRect.getDim(wallAxis.getPerpAxis()) >= narrowest)
        _roomRect.setPoints(&newRoomRect.topLeft, &newRoomRect.botRight);
    else
        return false;
    
    return true;//if we got this far, we're good
}


#pragma mark -
#pragma mark Doers - Graphics Functions

char CSRoom::assumeChar(CSEntity *inEnt, char inChar)
{
    if(inEnt == nullptr)
        return inChar;
    else
        return inEnt->getChar();
}

CSEntity* CSRoom::checkForEntityToDraw(CSPoint *inLoc)//finds and returns a creature or an entity or nothing
{
    CSEntity   *entToDraw = nullptr;
    
    list<CSEntity *>           entsInTile;
    list<CSEntity *>::iterator listIter;
    
    //if(*_theGame->getPlayer()->getLoc() == *inLoc)//player supercedes inanimate entities
        //return _theGame->getPlayer();
    
    for(listIter = _entities.begin(); listIter != _entities.end(); listIter++)
        if(*(*listIter)->getLoc() == *inLoc)
            entsInTile.push_back(*listIter);
    
    //if both a creature and a non-creature entity are found, make sure the creature supercedes the non-creature
    for(listIter = entsInTile.begin(); listIter != entsInTile.end(); listIter++)
    {
        entToDraw = *listIter;
        if((*listIter)->getType() == ENT_CREATURE)
            break;
    }
    
    return entToDraw;//if nothing was found, this will be null
}

string CSRoom::printRoomRow(CSRange *printRange, int rowToPrint)
{
    int     leftPrintBound, rightPrintBound;
    CSPoint tileToPrint;
    string  printString = "";
    bool    printLeftWall, printRightWall;
    
    //determine what part of the room is visible within the window, and set bounds accordingly
    if(printRange->getMin() < _roomRect.topLeft.x)
        leftPrintBound = _roomRect.topLeft.x;
    else
        leftPrintBound = printRange->getMin();
    printLeftWall = printRange->getMin() < _roomRect.topLeft.x;
    
    if(printRange->getMax() > _roomRect.botRight.x)
        rightPrintBound = _roomRect.botRight.x;
    else
        rightPrintBound = printRange->getMax();
    printRightWall = printRange->getMax() >= _roomRect.botRight.x;
    
    tileToPrint.y = rowToPrint;
    
    //use rowToPrint to determine which horizontal line of the room to print
    if(rowToPrint == _roomRect.topLeft.y || rowToPrint == _roomRect.botRight.y)//print top or bottom wall
        for(tileToPrint.x = leftPrintBound; tileToPrint.x <= rightPrintBound; tileToPrint.x++)
            printString += assumeChar(checkForEntityToDraw(&tileToPrint), WALL_CHAR);//send in the tile we wish to print and the char we assume to be there, checkForEntityToDraw will return the assumed tile or any overridden tile, based on room info, and assumeChar will find the correct char for that entity. Finally, we append it to printString.
    else//print the left wall, guts of the room, and right wall
    {
        //print left wall/door
        if(_roomRect.getWidth() > 0 && printLeftWall)
        {
            tileToPrint.x = _roomRect.topLeft.x;
            printString += assumeChar(checkForEntityToDraw(&tileToPrint), WALL_CHAR);
        }
        
        //print floor and/or entities
        for(tileToPrint.x = leftPrintBound + printLeftWall; tileToPrint.x <= rightPrintBound - printRightWall; tileToPrint.x++)//inset by 1 on each side for the walls
            printString += assumeChar(checkForEntityToDraw(&tileToPrint), FLOOR_CHAR);
        
        //print right wall/door
        if(_roomRect.getWidth() > 1 && printRightWall)//left wall and right wall
        {
            tileToPrint.x = _roomRect.botRight.x;
            printString += assumeChar(checkForEntityToDraw(&tileToPrint), WALL_CHAR);
        }
    }
    
    return printString;
}

string CSRoom::printRoomToFile(void)
{
    string  outputString;
    list<CSEntity *>::iterator listIter;
    
    //print basic room info
    outputString += _roomDataKey[0];
    outputString += ":";
    outputString += to_string(_roomNum);
    outputString += "\n";
    outputString += _roomDataKey[1];
    outputString += ":";
    outputString += to_string(_roomRect.topLeft.x);
    outputString += "\n";
    outputString += _roomDataKey[2];
    outputString += ":";
    outputString += to_string(_roomRect.topLeft.y);
    outputString += "\n";
    outputString += _roomDataKey[3];
    outputString += ":";
    outputString += to_string(_roomRect.botRight.x);
    outputString += "\n";
    outputString += _roomDataKey[4];
    outputString += ":";
    outputString += to_string(_roomRect.botRight.y);
    outputString += "\n";
    outputString += _roomDataKey[5];
    outputString += ":";
    outputString += to_string(_isHall);
    outputString += "\n";
    outputString += _roomDataKey[6];
    outputString += ":";
    outputString += to_string(_vertHall);
    outputString += "\n";
    
    //only print entities if there are any
    if(_entities.size() > 0)
    {
        outputString += FILE_DESCEND_CHAR;
        outputString += "\n";
        
        for(listIter = _entities.begin(); listIter != _entities.end(); listIter++)
        {
            if(!(*listIter)->getIsPlayer())//the dungeon will take care of saving the player state
                outputString += (*listIter)->printEntityToFile();
            if(*listIter == _entities.back())//we're done, so we're going back up
                outputString += FILE_ASCEND_CHAR;
            else//we have more entities
                outputString += FILE_TERM_CHAR;
            outputString += "\n";
        }
    }
    else//print info to set up next room
    {
        outputString += FILE_TERM_CHAR;
        outputString += "\n";
    }
    
    return outputString;
}


#pragma mark -
#pragma mark Complex Check Functions

CSEntity* CSRoom::getUnconnectedDoor(void)
{
    list<CSEntity *>::iterator entListIter;
    
    for(entListIter = _entities.begin(); entListIter != _entities.end(); entListIter++)
        if((*entListIter)->getType() == ENT_DOOR && (*entListIter)->getConnect() == nullptr)//this is a door & it should connect to our new room
            return *entListIter;
    
    return nullptr;
}

CSEntity* CSRoom::getConnectedDoor(void)
{
    list<CSEntity *>::iterator entListIter;
    
    for(entListIter = _entities.begin(); entListIter != _entities.end(); entListIter++)
        if((*entListIter)->getType() == ENT_DOOR && ((*entListIter)->getConnect() != nullptr))//this is a door & it should connect to our new room
            return (*entListIter);
    
    return nullptr;
}

CSEntity* CSRoom::getDoorConnectedToRoom(CSRoom *inRoom)
{
    list<CSEntity *>::iterator entListIter;
    
    for(entListIter = _entities.begin(); entListIter != _entities.end(); entListIter++)
    {
        if((*entListIter)->getConnect() == nullptr)
            continue;
        
        if((*entListIter)->getType() == ENT_DOOR && (*entListIter)->getConnect()->getOwner() == inRoom)
            return *entListIter;
    }
    
    return nullptr;
}

CSEntity* CSRoom::getEntityAtTile(CSPoint *inTile)//finds and returns a CSEntity, ignoring any creartures that might also be there
{
    list<CSEntity *>::iterator listIter;
    
    for(listIter = _entities.begin(); listIter != _entities.end(); listIter++)
        if(*(*listIter)->getLoc() == *inTile && (*listIter)->getType() != ENT_CREATURE && (*listIter)->getType() != ENT_ROOM_NUM)
            return *listIter;
    
    return nullptr;
}

bool CSRoom::getGoodRoomPoint(CSPoint &inPoint, bool isPassable)
{
    bool            goodLoc = false;
    int             loop, loopCounter = 0;
    CSAxis          adjacentAxis;
    CSRange         dimRange;
    CSPoint         adjacentLoc;
    CSRect          wallessRect;
    CSRandomRange   dimLocPoint;
    
    dimLocPoint.setRandType(RAND_ROOM);
    getWallessRect(wallessRect);
    
    while(!goodLoc)
    {
        for(loop = AXIS_HORIZ; loop <= AXIS_VERT; loop++)
        {
            wallessRect.getAxisRange((axis)loop, dimRange);
            dimLocPoint.setRange(dimRange);
            _theGame->theRandHand.addRandomRange(dimLocPoint);
            
            inPoint.setAxisPoint((axis)loop, _theGame->theRandHand.getNumber(&dimLocPoint));//set the loc point to a random point in the room
        }
        
        if(getEntityAtTile(&inPoint) == nullptr)//make sure no entities already exists at this location
            goodLoc = true;
        
        if(!isPassable)//if this entity type is impassable...
        {
            //check the 4 adjacent tiles for doors
            for(loop = REG_WALL_LEFT; loop <= REG_WALL_BOT; loop++)
            {
                adjacentAxis.setAxisFromWall((entReg)loop);
                adjacentLoc = inPoint;
                adjacentLoc.slidePointViaAxis(adjacentAxis.dim, 1 * adjacentAxis.getDirOffset());
                
                if(getEntityAtTile(&adjacentLoc) != nullptr)
                    if(getEntityAtTile(&adjacentLoc)->getType() == ENT_DOOR)//we are next to a door
                    {
                        goodLoc = false;
                        break;
                    }
            }
        }
        
        loopCounter++;
        if(loopCounter >= 5)//semi-arbitrary limit
            return false;
    }
    
    _theGame->theRandHand.clearRandomItems(RAND_ROOM);
    return true;
}

bool CSRoom::isWallPointFree(CSPoint *inPoint, entReg inWall, CSEntity *doorToMove)
{
    CSAxis  wallAxis;
    
    list<CSEntity*>::iterator  entIter;
    
    wallAxis.setAxisFromWall(inWall);
    
    //if inPoint is on either corner, or is outside the wall range alltogether...
    if(inPoint->getAxisPoint(wallAxis.dim) <= getRect()->getWallStartPoint(inWall) ||
       inPoint->getAxisPoint(wallAxis.dim) >= getRect()->getWallEndPoint(inWall))
        return false;
    
    //check every door on inWall
    for(entIter = _entities.begin(); entIter != _entities.end(); entIter++)
    {
        int entNum;
        entNum = (*entIter)->getNum();
        
        //skip doors on other walls, non-doors, or the door we're moving that's leading to this wall check
        if((*entIter)->getType() != ENT_DOOR || (*entIter)->getRegion() != inWall || (*entIter) == doorToMove)
            continue;
        
        //if inPoint is where inWall has a connected door
        if(inPoint->getAxisPoint(wallAxis.dim) == (*entIter)->getLoc()->getAxisPoint(wallAxis.dim))
            return false;
        
        //if inPoint aligns with one of the walls of a connected hallway
        if(inPoint->getAxisPoint(wallAxis.dim) == (*entIter)->getLoc()->getAxisPoint(wallAxis.dim) - (HALL_SIZE / 2) ||
           inPoint->getAxisPoint(wallAxis.dim) == (*entIter)->getLoc()->getAxisPoint(wallAxis.dim) + (HALL_SIZE / 2))
            return false;
    }
    
    return true;
}

bool CSRoom::isTilePassable(CSPoint *inTile)
{
    bool        insideWalls;
    CSRect      wallessRect;
    CSEntity   *tileEnt = nullptr;
    
    //check whether there is a wall at the tile (if the new tile is in _roomRect but NOT in wallesRect)
    getWallessRect(wallessRect);
    insideWalls = wallessRect.doesRectContainPoint(inTile);
    
    tileEnt = checkForEntityToDraw(inTile);//check if there's an entity or creature at this tile
    if(tileEnt != nullptr)//if there is, return whether that entity is passable
    {
        if(tileEnt->isPassable())
        {
            if(tileEnt->getType() == ENT_ROOM_NUM)
                return insideWalls;
            else
                return true;
        }
        else
        {
            tileEnt->activateAdjacent();
            return false;
        }
    }
    else
        return insideWalls;
}


#pragma mark -
#pragma mark Getters

bool CSRoom::isHall(void)
{
    return _isHall;
}

bool CSRoom::isVertHall(void)
{
    return _vertHall;
}

int CSRoom::getRoomNum(void)
{
    return _roomNum;
}

CSRandomRange* CSRoom::getWallGenRanges(void)
{
    return &_wallGenLoc[0];
}

list<CSEntity*>* CSRoom::getEntities(void)
{
    return &_entities;
}

CSEntity* CSRoom::getEntityWithNum(int inEntNum)
{
    list<CSEntity *>::iterator listIter;
    
    for(listIter = _entities.begin(); listIter != _entities.end(); listIter++)
    {
        if((*listIter)->getNum() == inEntNum)
            return *listIter;
    }
    
    return nullptr;
}

CSRect* CSRoom::getRect(void)
{
    return &_roomRect;
}

void CSRoom::getWallessRect(CSRect &inRect)
{
    int     newWallLoc, loop;
    CSAxis  wallAxis;
    
    inRect = _roomRect;
    
    for(loop = REG_WALL_LEFT; loop <= REG_WALL_BOT; loop++)
    {
        wallAxis.setAxisFromWall((entReg)loop);
        newWallLoc = inRect.getWallLocPoint((entReg)loop);
        newWallLoc += 1 * wallAxis.getOppDirOffset();//increase on top/left, decreases on bot/right--thus always sliding in from wall
        inRect.setWallLoc((entReg)loop, newWallLoc);
    }
}

CSRoom* CSRoom::getRoomToConnect(void)
{
    return _roomToConnect;
}

int CSRoom::getNumDoors(void)
{
    return _numDoors;
}
