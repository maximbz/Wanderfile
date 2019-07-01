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


int main(void)
{
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
    
    menuWind = newwin(MENU_BOUND_HEIGHT, theGame.getGameWindRect()->getWidth(), theGame.getGameWindRect()->getWidth(),
                      theGame.getGameWindRect()->getHeight() + MENU_BOUND_HEIGHT);
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
                
            mvwaddstr(menuWind, WINDOW_BOUND_BOTTOM - MENU_BOUND_HEIGHT, 0, "\n\nMove Player Chracter a - Left, d - Right, w - Up, s - Down.");
            mvwaddstr(menuWind, WINDOW_BOUND_BOTTOM - MENU_BOUND_HEIGHT + 1, 0, "\nOR: Create (N)ew dungeon, Toggle line (B)reak, or (Q)uit?\n");
            
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
