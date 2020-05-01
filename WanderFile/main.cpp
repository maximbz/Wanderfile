//
//  main.cpp
//  WanderFile
//
//  Created by Maxim Boschert-Zielsdorf on 1/24/17.
//  Copyright © 2017 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include <cstdio>
#include <vector>
#include <ncurses.h>
#include "WanderFile.h"
#include "CSPlayerChoice.hpp"
#include "CSRandomHandler.hpp"
#include "CSDoorHandler.hpp"
#include "CSDungeonLevel.hpp"
#include "CSGameState.hpp"

using namespace std;


int main(int argc, const char * argv[])
{
    for (int argi = 1; argi < argc; argi++)
    {
        if (strcmp(argv[argi], "--debug-in-terminal") == 0)
        {
            printf("Debugging in terminal enabled\n");
            getchar(); // Without this call debugging will be skipped
            break;
        }
    }
    
    bool            gameLoop = true, printLoop;
    int             numDungeons = -1;
    string          outputString;
    vector<char>    gameOptions, slideOptions, mainModeOptions;
    WINDOW          *menuWind;
    CSPoint         menuSelectMatrix;
    CSRange         roomNumRange;
    CSPlayerChoice  menuSelection;
    
    static CSRandomHandler theRandHand;
    static CSDoorHandler   theDoorHand(&theRandHand);
    static CSGameState     theGame;
    
    CSDungeonLevel  dungeon(&theRandHand, &theGame, &theDoorHand, 1);
    
    list<CSRoom *>::iterator    roomListIter;
    
    menuWind = newwin(MENU_BOUND_HEIGHT, theGame.getGameWindRect()->getWidth(), theGame.getGameWindRect()->getHeight() + 1, 0);
    menuWind = initscr();//initializes terminal to use ncurses
    cbreak();//disable the buffering of typed characters by the TTY driver and get a character-at-a-time input
    noecho();//keeps typed keys from automatically echoing to terminal
    clear();
    refresh();
    curs_set(0);//set curse to be invisible
    start_color();
    
    slideOptions.push_back('a');
    slideOptions.push_back('d');
    slideOptions.push_back('w');
    slideOptions.push_back('s');
    
    mainModeOptions.push_back('n');
    mainModeOptions.push_back('l');
    mainModeOptions.push_back('v');
    
    gameOptions.push_back('b');
    gameOptions.push_back('q');
    
    menuSelection.addCharVect(&slideOptions);
    menuSelection.addCharVect(&mainModeOptions);
    menuSelection.addCharVect(&gameOptions);
    
    //loop
    while(gameLoop)
    {
        printLoop = true;
        
        while(printLoop)
        {
            dungeon.printWindow();
            
            mvwaddstr(menuWind, WINDOW_BOUND_BOTTOM + 1, 0, outputString.c_str());
            mvwaddstr(menuWind, WINDOW_BOUND_BOTTOM + 2, 0, "(L)oad dungeon, Create (N)ew dungeon, Sa(V)e current dungeon, Toggle line (B)reak, or (Q)uit?\n");
            
            menuSelection.toggleCharOption(1, true);//turn main mode on
            menuSelection.getUserCharAnswer(menuSelectMatrix, menuWind);
            
            //directions
            if(menuSelectMatrix.x == 0)
            {
                /*if(menuSelectMatrix.y == 0)
                    for(loop = 0; loop < 3; loop++)
                        theGame.slideGameWindow(REG_WALL_LEFT);
                if(menuSelectMatrix.y == 1)
                    for(loop = 0; loop < 3; loop++)
                        theGame.slideGameWindow(REG_WALL_RIGHT);
                if(menuSelectMatrix.y == 2)
                    for(loop = 0; loop < 3; loop++)
                        theGame.slideGameWindow(REG_WALL_TOP);
                if(menuSelectMatrix.y == 3)
                    for(loop = 0; loop < 3; loop++)
                        theGame.slideGameWindow(REG_WALL_BOT);*/
                if(menuSelectMatrix.y == 0)
                    dungeon.movePlayer(REG_WALL_LEFT);
                if(menuSelectMatrix.y == 1)
                    dungeon.movePlayer(REG_WALL_RIGHT);
                if(menuSelectMatrix.y == 2)
                    dungeon.movePlayer(REG_WALL_TOP);
                if(menuSelectMatrix.y == 3)
                    dungeon.movePlayer(REG_WALL_BOT);
            }
            
            //main menu commands
            if(menuSelectMatrix.x == 1)
            {
                if(menuSelectMatrix.y == 0)
                {
                    dungeon.createDungeon();
                    numDungeons++;
                }
                if(menuSelectMatrix.y == 1)
                {
                    dungeon.loadDungeon(&outputString);
                    numDungeons++;
                }
                if(menuSelectMatrix.y == 2)
                    dungeon.saveDungeon();
            }
            
            //game commends
            if(menuSelectMatrix.x == 2)
            {
                if(menuSelectMatrix.y == 0)//toggle line break
                {
                    theGame.toggleBreak();
                    
                    if(theGame.getBreakState() == true)
                        numDungeons = numDungeons;
                }
                if(menuSelectMatrix.y == 1)//quit
                {
                    gameLoop = false;
                    printLoop = false;
                }
            }
        }
        
        dungeon.deleteDungeon();
    }
    
    theGame.cleanUpGameState();
    return 0;
}
