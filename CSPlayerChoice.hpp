//
//  CSPlayerChoice.hpp
//  Tic Tac Toe
//
//  Created by Maxim Boschert-Zielsdorf on 9/14/16.
//  Copyright © 2016 Maxim Boschert-Zielsdorf. All rights reserved.
//

#ifndef CSPlayerChoice_hpp
#define CSPlayerChoice_hpp

#include <vector>
#include "CSRange.hpp"
#include "CSPoint.hpp"

using namespace std;

class CSPlayerChoice
{
private:
    bool                    _hasInt, _hasChar;
    vector<bool>            _charOptionStates;
    CSRange                 _intRange;
    vector< vector<char> >  _charOptions;//x = 
    
public:
    CSPlayerChoice();
    CSPlayerChoice(CSRange);
    CSPlayerChoice(vector<char>);
    CSPlayerChoice(CSRange, vector<char>);
    
    void setIntRange(CSRange);
    void addCharVect(vector<char>);
    void toggleCharOption(int, bool);
    
    CSPoint getUserCharAnswer(void);
    int getUserIntAnswer(void);
    CSPoint getUserMixedAnswer(void);
    void printOptions(void);
    CSPoint parseResponse(string &);
    
    //will we need getters?
};

#endif /* CSPlayerChoice_hpp */
