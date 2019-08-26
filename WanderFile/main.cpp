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
#include "RandomHandler/CSRandomHandler.hpp"
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
    vector<char>    gameOptions, slideOptions, mainModeOptions;
    WINDOW          *menuWind;
    CSPoint         menuSelectMatrix;
    CSRange         roomNumRange;
    CSPlayerChoice  menuSelection;
    CSRandomHandler theRandHand;
    CSDoorHandler   theDoorHand(&theRandHand);
    CSGameState     theGame;
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
    
    gameOptions.push_back('b');
    gameOptions.push_back('q');
    
    menuSelection.addCharVect(&slideOptions);
    menuSelection.addCharVect(&mainModeOptions);
    menuSelection.addCharVect(&gameOptions);
    
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
            dungeon.printWindow();
            
            //temp debug block to display player loc
            string  outputStr = "Updated Rooms: ";//"Player Movement: a-Left, d-Right, w-Up, s-Down."
            for(roomListIter = dungeon.roomsToUpdate.begin(); roomListIter != dungeon.roomsToUpdate.end(); roomListIter++)
            {
                outputStr.append(to_string((*roomListIter)->getRoomNum()));
                outputStr.append(",");
            }
            
            mvwaddstr(menuWind, WINDOW_BOUND_BOTTOM + 1, 0, outputStr.c_str());
            mvwaddstr(menuWind, WINDOW_BOUND_BOTTOM + 2, 0, "OR: Create (N)ew dungeon, Toggle line (B)reak, or (Q)uit.\n");
            
            menuSelection.toggleCharOption(1, true);//turn main mode on
            menuSelection.getUserCharAnswer(menuSelectMatrix, menuWind);
            
            //directions
            if(menuSelectMatrix.x == 0)
            {
                if(menuSelectMatrix.y == 0)
                    dungeon.movePlayer(-1, 0);
                if(menuSelectMatrix.y == 1)
                    dungeon.movePlayer(1, 0);
                if(menuSelectMatrix.y == 2)
                    dungeon.movePlayer(0, -1);
                if(menuSelectMatrix.y == 3)
                    dungeon.movePlayer(0, 1);
            }
            
            //main menu commands
            if(menuSelectMatrix.x == 1)
            {
                if(menuSelectMatrix.y == 0)//quit
                    printLoop = false;
                if(menuSelectMatrix.y == 1)//new dungeon
                    dungeon.createDungeon();
            }
            
            //game commends
            if(menuSelectMatrix.x == 2)
            {
                if(menuSelectMatrix.y == 0)//toggle line break
                    theGame.toggleBreak();
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
