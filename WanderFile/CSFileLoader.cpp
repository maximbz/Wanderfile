//
//  CSFileLoader.cpp
//  WanderFile
//
//  Created by Maxim Boschert Zielsdorf on 10/8/19.
//  Copyright © 2019 Maxim Boschert-Zielsdorf. All rights reserved.
//

#include "WanderFile.h"
#include "CSFileLoader.hpp"
#include <fstream>
#include <sstream>
#include <vector>

CSFileLoader::CSFileLoader()
{
    _completeState = FILE_NULL_CHAR;
}

CSFileLoader::CSFileLoader(string *inFile, int inParseResult)
{
    _fileName = *inFile;
    _completeState = FILE_NULL_CHAR;
    
    inParseResult = inputFileData();
}


int CSFileLoader::inputFileData(void)
{
    ifstream    inputFile;
    string      inputString;
    
    //open the file and check for errors
    inputFile.open(_fileName);
    if(inputFile.fail())
    {
        perror(_fileName.c_str());
        return 1;
    }
    
    //pull each line of the file into stringVect
    while(getline(inputFile, inputString))
        _fileData.push_back(inputString);
    
    _empty = false;
    
    return 0;
}

void CSFileLoader::populateDictionary(void)
{
    bool        key;
    int         loop, numLinesRead = 0;
    string      inputString, keyString, valueString;
    
    list<string>::iterator          listIter;
    
    _dictionary.clear();
    _completeState = FILE_NULL_CHAR;
    
    //each for-loop is a line in _fileData
    for(listIter = _fileData.begin(); listIter != _fileData.end(); listIter++)
    {
        //set our shit up
        _completeState = FILE_NULL_CHAR;
        inputString = *listIter;
        keyString = "";
        valueString = "";
        key = true;
        
        //each for-loop is a character on a line--parse through inputString string and populate key and value strings
        for(loop = 0; loop < inputString.size(); loop++)
        {
            if(inputString[loop] == FILE_TERM_CHAR || inputString[loop] == FILE_ASCEND_CHAR || inputString[loop] == FILE_DESCEND_CHAR)
            {
                _completeState = inputString[loop];
                break;//we'll only hit this character if we've already gone through all the other values
            }
            else if(inputString[loop] == ':')
                key = false;
            else if(key)
                keyString += inputString[loop];//add one char to the growing key string
            else
                valueString += inputString[loop];//add one char to the growing value string
        }

        numLinesRead++;//whether a file char or a new key-value pair, we need to record it, so we can delete it later
        
        if(_completeState == FILE_NULL_CHAR)//we have a normal key-value pair
            _dictionary.insert(pair<string, string>(keyString, valueString));
        else
            break;//stop pulling from _fileData, or we'll screw up the next guy
    }
    
    //now that we've populated _dictionary, we can remove those lines from _fileData
    for(loop = 0; loop < numLinesRead; loop++)
        _fileData.pop_front();
    if(_fileData.size() == 0)
        _empty = true;
}

bool CSFileLoader::getIntValueFromKey(string inKey, int &outValue)
{
    string  stringToConvert;
    map<string, string>::iterator  mapIter;
    
    mapIter = _dictionary.find(inKey);
    if(mapIter != _dictionary.end())
    {
        stringToConvert = mapIter->second;
        istringstream strToInt(stringToConvert);
        strToInt >> outValue;
        
        return true;
    }
    
    return false;
}

bool CSFileLoader::getStringValueFromKey(string inKey, string &outValue)
{
    map<string, string>::iterator  mapIter;
    
    mapIter = _dictionary.find(inKey);
    if(mapIter != _dictionary.end())
    {
        outValue = mapIter->second;
        
        return true;
    }
    
    return false;
}

char CSFileLoader::getCompleteState(void)
{
    return _completeState;
}

bool CSFileLoader::isEmpty(void)
{
    return _empty;
}

