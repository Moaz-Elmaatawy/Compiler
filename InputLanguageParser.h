#include "Token.h"
#include <fstream>
#include <set>
#include <map>
#include <vector>

using namespace std;

#ifndef INPUTLANGUAGEPARSER_H
#define INPUTLANGUAGEPARSER_H

class InputLanguageParser {
private:
    map<set<int>, string> acceptingStates;
    map<set<int>, map<string, set<int>>> transitions;
    set<int> startStates;
    fstream file;
    vector<Token> tokenize(const string &line);

public:
    InputLanguageParser();
    InputLanguageParser(string path, map<set<int>, map<string, set<int>>> transitions, map<set<int>, string> acceptingStates, set<int> startStates);
    vector<Token> parseFile();
};
#endif // INPUTLANGUAGEPARSER_H
