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


#pragma mark -
#pragma mark Constructors

CSPlayerChoice::CSPlayerChoice()
{
}

CSPlayerChoice::CSPlayerChoice(CSRange *inRange)
{
    _hasInt = true;
    _hasChar = false;
    
    _intRange = *inRange;
}

CSPlayerChoice::CSPlayerChoice(vector<char> *inCharOptions)
{
    _hasInt = false;
    _hasChar = true;
    
    _charOptions.push_back(*inCharOptions);
    _charOptionStates.push_back(true);
}

CSPlayerChoice::CSPlayerChoice(CSRange inRange, vector<char> *inCharOptions)
{
    _hasInt = true;
    _hasChar = true;
    
    _intRange = inRange;
    
    _charOptions.push_back(*inCharOptions);
    _charOptionStates.push_back(true);
}


#pragma mark -
#pragma mark Setters

void CSPlayerChoice::setIntRange(CSRange *inRange)
{
    _intRange = *inRange;
}

void CSPlayerChoice::addCharVect(vector<char> *inCharOptions)
{
    _charOptions.push_back(*inCharOptions);
    _charOptionStates.push_back(true);
}

void CSPlayerChoice::toggleCharOption(int inCharOption, bool inState)
{
    _charOptionStates[inCharOption] = inState;
}


#pragma mark -
#pragma mark Doers


void CSPlayerChoice::printOptions(WINDOW *inWind)
{
    int loop, subloop;
    
    //go through vector and print it, formatted
    for(loop = 0;loop < _charOptions.size(); loop++)
    {
        if(!_charOptionStates[loop])//don't check char option sets that have been turned off
            continue;
        
        for(subloop = 0;subloop < _charOptions[loop].size(); subloop++)
        {
            waddch(inWind, _charOptions[loop][subloop]);
            if(subloop < _charOptions[loop].size() - 1)//if not last loop
                waddstr(inWind, ", ");
        }
        
        if(loop < _charOptions.size() - 1)//if not last loop
            waddstr(inWind, ", ");
    }
    waddstr(inWind, ")  ");
}

void CSPlayerChoice::parseResponse(char inResponse, CSPoint &inPoint)
{
    int loop, subloop;
    
    for(loop = 0; loop < _charOptions.size(); loop++)
    {
        if(!_charOptionStates[loop])//don't check char option sets that have been turned off
            continue;
        
        for(subloop = 0; subloop < _charOptions[loop].size(); subloop++)
            if(inResponse == _charOptions[loop][subloop])
            {
                inPoint.setPoints(loop, subloop);
                loop = (int)_charOptions.size();//get us out of the outer loop
                break;
            }
    }
}

void CSPlayerChoice::getUserCharAnswer(CSPoint &inPoint, WINDOW *inWind)
{
    char    userResponse;
    CSPoint responseMatrix(BAD_DATA, BAD_DATA);
    
    //print out letter options
    //waddch(inWind, '(');//no leading parens in printOptions
    //printOptions(inWind);
    
    userResponse = wgetch(inWind);
    userResponse = tolower(userResponse);
    
    parseResponse(userResponse, responseMatrix);//go through again and determine if user input matches any of the chars
    
    inPoint = responseMatrix;
}

int CSPlayerChoice::getUserIntAnswer(WINDOW *inWind)
{
    int     userResponse = BAD_DATA;
    bool    goodResponse = false;
    string  rangeString;
    
    while(!goodResponse)
    {
        waddch(inWind, '(');
        rangeString = to_string(_intRange.getMin());
        waddstr(inWind, rangeString.c_str());
        rangeString = to_string(_intRange.getMax());
        waddstr(inWind, rangeString.c_str());
        waddch(inWind, ')');
        
        userResponse = wgetch(inWind);
        userResponse = tolower(userResponse);
        
        goodResponse = _intRange.doesContain(userResponse);
    }
    
    return userResponse;
}


