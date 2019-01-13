//
//  main.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 1/24/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include <cstdio>
#include <vector>
#include "WanderFile.h"
#include "CSPlayerChoice.hpp"
#include "CSRandomHandler.hpp"
#include "CSDoorHandler.hpp"
#include "CSDungeonLevel.hpp"
#include "CSGameState.hpp"

using namespace std;


int main(void)
{
    CSRandomHandler theRandHand;
    CSGameState     theGame;
    CSDoorHandler   theDoorHand(&theRandHand);
    CSDungeonLevel  dungeon(&theRandHand, &theGame, &theDoorHand, 1);
    CSPlayerChoice  menuSelection;
    vector<char>    gameOptions, slideOptions, mainModeOptions;
    CSPoint         menuSelectMatrix;
    CSRange         roomNumRange;
    bool            gameLoop = true, printLoop, slideRoom = false, loopForever = false;
    int             numDungeons = -1, slideRoomNum = -1;
    
    slideOptions.push_back('a');
    slideOptions.push_back('d');
    slideOptions.push_back('w');
    slideOptions.push_back('s');
    
    mainModeOptions.push_back('n');
    mainModeOptions.push_back('r');
    mainModeOptions.push_back('e');
    mainModeOptions.push_back('o');
    
    gameOptions.push_back('l');
    gameOptions.push_back('b');
    gameOptions.push_back('q');
    
    menuSelection.addCharVect(slideOptions);
    menuSelection.addCharVect(mainModeOptions);
    menuSelection.addCharVect(gameOptions);
    
    //loop
    while(gameLoop)
    {
        dungeon.createDungeon();
        numDungeons++;
        printLoop = true;
        
        //if(dungeon.saveDungeon())
            //return 1;
        
        while(printLoop)
        {
            //printf("%d\n", numDungeons);
            //printf("12345678 112345678 212345678 312345678 412345678 512345678 612345678 712345678 812345678 912345678 0\n");//pseudo-grid
            dungeon.printWindow();
            
            if(!loopForever)
            {
                if(!slideRoom)
                {
                    printf("\n\nSlide Game Window a - Left, d - Right, w - Up, s - Down.");
                    printf("\nOR: Create (N)ew dungeon, Add new (R)oom, D(E)lete newest Room, Replace newest D(O)or, Enter S(L)ide Room Mode, Toggle line (B)reak, or (Q)uit?\n");
                    
                    menuSelection.toggleCharOption(1, true);//turn main mode on
                    menuSelectMatrix = menuSelection.getUserCharAnswer();
                    
                    //directions
                    if(menuSelectMatrix.x == 0)
                    {
                        if(menuSelectMatrix.y == 0)
                            theGame.slideGameWindow(-10, 0);
                        if(menuSelectMatrix.y == 1)
                            theGame.slideGameWindow(10, 0);
                        if(menuSelectMatrix.y == 2)
                            theGame.slideGameWindow(0, -10);
                        if(menuSelectMatrix.y == 3)
                            theGame.slideGameWindow(0, 10);
                    }
                    
                    //main menu commands
                    if(menuSelectMatrix.x == 1)
                    {
                        if(menuSelectMatrix.y == 0)
                            printLoop = false;
                        if(menuSelectMatrix.y == 1)
                            dungeon.createDungeon();
                        if(menuSelectMatrix.y == 2)
                            dungeon.deleteRoom(nullptr);//delete last room
                        if(menuSelectMatrix.y == 3)
                            dungeon.replaceDoor();
                    }
                    
                    //game commends
                    if(menuSelectMatrix.x == 2)
                    {
                        if(menuSelectMatrix.y == 0)//enter slide mode
                        {
                            slideRoom = true;
                            slideRoomNum = dungeon.getNumRooms() - 1;
                        }
                        if(menuSelectMatrix.y == 1)
                            theGame.toggleBreak();
                        if(menuSelectMatrix.y == 2)//quit
                        {
                            gameLoop = false;
                            printLoop = false;
                        }
                        /*if(userResponse == 'l')
                        {
                            printLoop = false;
                            loopForever = true;
                        }*/
                    }
                }
                else//slideRoom
                {
                    printf("\n\nSlide Room Mode: Current room to slide is %02d. a - Left, d - Right, w - Up, s - Down.", slideRoomNum);
                    printf("\nOR: Enter new room number, (L)eave Slide Room Mode, Toggle line (B)reak, or (Q)uit?\n");
                    
                    menuSelection.toggleCharOption(1, false);//turn main mode off
                    roomNumRange.setRange(0, dungeon.getNumRooms() - 1);
                    menuSelection.setIntRange(roomNumRange);
                    menuSelectMatrix = menuSelection.getUserMixedAnswer();
                    
                    //room sliding directions
                    if(menuSelectMatrix.x == 0)
                    {
                        if(menuSelectMatrix.y == 0)
                            dungeon.slideRoom(slideRoomNum, -1, 0);
                        if(menuSelectMatrix.y == 1)
                            dungeon.slideRoom(slideRoomNum, 1, 0);
                        if(menuSelectMatrix.y == 2)
                            dungeon.slideRoom(slideRoomNum, 0, -1);
                        if(menuSelectMatrix.y == 3)
                            dungeon.slideRoom(slideRoomNum, 0, 1);
                    }
                    
                    //game commends
                    if(menuSelectMatrix.x == 2)
                    {
                        if(menuSelectMatrix.y == 0)//leave slide mode
                            slideRoom = false;
                        if(menuSelectMatrix.y == 1)
                            theGame.toggleBreak();
                        if(menuSelectMatrix.y == 2)//quit
                        {
                            gameLoop = false;
                            printLoop = false;
                        }
                    }
                    
                    //room selection
                    if(menuSelectMatrix.x == 3)
                        slideRoomNum = menuSelectMatrix.y;
                }
                
                while (getchar() != '\n');//clear buffer
            }
            else
                printLoop = false;
        }
        
        dungeon.deleteDungeon();
        
    }
    
    return 0;
}
