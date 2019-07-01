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
#include <ncurses.h>
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
    CSPlayerChoice(CSRange *);
    CSPlayerChoice(vector<char> *);
    CSPlayerChoice(CSRange, vector<char> *);
    
    void setIntRange(CSRange *);
    void addCharVect(vector<char> *);
    void toggleCharOption(int, bool);
    
    void printOptions(WINDOW *);
    void parseResponse(char, CSPoint &);
    
    void getUserCharAnswer(CSPoint &, WINDOW *);
    int getUserIntAnswer(WINDOW *);
    void getUserMixedAnswer(CSPoint &, WINDOW *);
};

#endif /* CSPlayerChoice_hpp */
