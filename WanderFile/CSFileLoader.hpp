//
//  CSFileLoader.hpp
//  WanderFile
//
//  Created by Maxim Boschert Zielsdorf on 10/8/19.
//  Copyright © 2019 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSFileLoader_hpp
#define CSFileLoader_hpp

#include <stdio.h>
#include <string>
#include <list>
#include <map>

using namespace std;


class CSFileLoader
{
private:
    bool                _empty;
    char                _completeState;
    string              _fileName;
    list<string>        _fileData;
    map<string, string> _dictionary;
    
    
    int inputFileData(void);//gets file into _fileData
    
public:
    CSFileLoader();
    CSFileLoader(string *, int);
    
    void populateDictionary(void);//loops through a section of _fileData
    
    bool getIntValueFromKey(string, int &);
    bool getStringValueFromKey(string, string &);
    char getCompleteState(void);
    bool isEmpty(void);
};

#endif /* CSFileLoader_hpp */
