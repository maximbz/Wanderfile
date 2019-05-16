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
    bool            gameLoop = true, printLoop;
    int             numDungeons = -1;
    
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
                
            printf("\n\nMove Player Chracter a - Left, d - Right, w - Up, s - Down.");
            printf("\nOR: Create (N)ew dungeon, Toggle line (B)reak, or (Q)uit?\n");
            
            menuSelection.toggleCharOption(1, true);//turn main mode on
            menuSelection.getUserCharAnswer(menuSelectMatrix);
            
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
            
            while (getchar() != '\n');//clear buffer
        }
        
        dungeon.deleteDungeon();
        
    }
    
    theGame.cleanUpGameState();
    
    return 0;
}
