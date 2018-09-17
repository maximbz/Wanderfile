//
//  CSPlayerChoice.cpp
//  Tic Tac Toe
//
//  Created by Maxim Boschert-Zielsdorf on 9/14/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "CSPlayerChoice.hpp"
#include <iostream>
#include <cstdio>
#include <cmath>

using namespace std;


#pragma mark -
#pragma mark Constructors

CSPlayerChoice::CSPlayerChoice()
{
}

CSPlayerChoice::CSPlayerChoice(CSRange inRange)
{
    _hasInt = true;
    _hasChar = false;
    
    _intRange = inRange;
}

CSPlayerChoice::CSPlayerChoice(vector<char> inCharOptions)
{
    _hasInt = false;
    _hasChar = true;
    
    _charOptions.push_back(inCharOptions);
    _charOptionStates.push_back(true);
}

CSPlayerChoice::CSPlayerChoice(CSRange inRange, vector<char> inCharOptions)
{
    _hasInt = true;
    _hasChar = true;
    
    _intRange = inRange;
    
    _charOptions.push_back(inCharOptions);
    _charOptionStates.push_back(true);
}


#pragma mark -
#pragma mark Setters

void CSPlayerChoice::setIntRange(CSRange inRange)
{
    _intRange = inRange;
}

void CSPlayerChoice::addCharVect(vector<char> inCharOptions)
{
    _charOptions.push_back(inCharOptions);
    _charOptionStates.push_back(true);
}

void CSPlayerChoice::toggleCharOption(int inCharOption, bool inState)
{
    _charOptionStates[inCharOption] = inState;
}


#pragma mark -
#pragma mark Doers

CSPoint CSPlayerChoice::getUserCharAnswer(void)
{
    string  userResponse(4, ' ');
    bool    goodResponse = false;
    CSPoint responseMatrix;
    
    while(!goodResponse)
    {
        printf("(");//no leading parens in printOptions
        printOptions();
        scanf("%c", &userResponse[0]);
        
        responseMatrix = parseResponse(userResponse);//go through again and determine if user input matches any of the chars
        
        if(responseMatrix.x != BAD_DATA && responseMatrix.y != BAD_DATA)
            goodResponse = true;
    }
    
    return responseMatrix;
}

int CSPlayerChoice::getUserIntAnswer(void)
{
    int     userResponse = BAD_DATA;
    bool    goodResponse = false;
    
    while(!goodResponse)
    {
        printf("(%d - %d)", _intRange.getMin(), _intRange.getMax());
        scanf("%d", &userResponse);
        
        goodResponse = _intRange.doesContain(userResponse);
    }
    
    return userResponse;
}

CSPoint CSPlayerChoice::getUserMixedAnswer(void)
{
    int     loop, loopSize, subloop;
    string  userResponse(4, ' ');
    bool    goodResponse = false;
    CSPoint responseMatrix;
    
    while(!goodResponse)
    {
        printf("(%d - %d OR: ", _intRange.getMin(), _intRange.getMax());//print number range first
        printOptions();//then go through vector and print it, formatted
        
        scanf("%s", &userResponse[0]);
        
        //check if the first character of userResponse matches any of the charOptions
        responseMatrix = parseResponse(userResponse);
        if(responseMatrix.x != BAD_DATA && responseMatrix.y != BAD_DATA)
            goodResponse = true;
        
        //if not, is it within the range?
        if(!goodResponse)
        {
            int     intPos = -1;
            bool    intFound = false;
            
            loopSize = (int)strlen(userResponse.c_str());
            
            ////check the beginning of userResponse for an integer
            for(loop = 0; loop < loopSize; loop++)
            {
                //loop through every digit, checking if it's at this index of userResponse
                for(subloop = '0'; subloop <= '9'; subloop++)
                    if(userResponse[loop] == subloop)
                    {
                        intFound = true;
                        break;//we found a digit, no more need for searching
                    }
                
                if(intFound)
                    intPos++;
                else
                    break;//anytime we hit a char, we stop looking
            }
            
            if(intFound)
            {
                int decimalPlace = 0;
                responseMatrix.setPoints((int)_charOptions.size(), 0);//an int user response is treated as an additional char vector
                
                //takes the integer we found in userReponse and adds it to responseInt
                for(loop = intPos; loop >= 0; loop--)//goes from the last number of the integer we found, leftward
                {
                    responseMatrix.y += (userResponse[loop] - '0') * pow(10, decimalPlace);//as we move right, we increase the power of ten
                    decimalPlace++;//as we move left in the string, we must move right in the int
                }
                
                //if it's actually a spot within the range, we can leave the while loop
                goodResponse = _intRange.doesContain(responseMatrix.y);
            }
        }
    }
    
    return responseMatrix;
}

void CSPlayerChoice::printOptions(void)
{
    int loop, subloop;
    
    //go through vector and print it, formatted
    for(loop = 0;loop < _charOptions.size(); loop++)
    {
        if(!_charOptionStates[loop])//don't check char option sets that have been turned off
            continue;
        
        for(subloop = 0;subloop < _charOptions[loop].size(); subloop++)
        {
            printf("%c", _charOptions[loop][subloop]);
            if(subloop < _charOptions[loop].size() - 1)//if not last loop
                printf(", ");
        }
        
        if(loop < _charOptions.size() - 1)//if not last loop
            printf(", ");
    }
    printf(")  ");
}

CSPoint CSPlayerChoice::parseResponse(string &inResponse)
{
    int     loop, subloop;
    CSPoint responseMatrix(BAD_DATA, BAD_DATA);
    
    for(loop = 0; loop < _charOptions.size(); loop++)
    {
        if(!_charOptionStates[loop])//don't check char option sets that have been turned off
            continue;
        
        for(subloop = 0;subloop < _charOptions[loop].size(); subloop++)
            if(inResponse[0] == _charOptions[loop][subloop])
            {
                responseMatrix.setPoints(loop, subloop);
                loop = (int)_charOptions.size();//get us out of the outer loop
                break;
            }
    }
    
    return responseMatrix;
}







