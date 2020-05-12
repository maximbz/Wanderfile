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
    
    static CSGameState theGame;
    
    //loop
    while(theGame.gameLoop())
    {}
    
    theGame.cleanUpGameState();
    return 0;
}
