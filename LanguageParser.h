#include "Token.h"
#include <fstream>
#include <set>
#include <map>
#include <vector>

using namespace std;

#ifndef LANGUAGEPARSER_H
#define LANGUAGEPARSER_H

class LanguageParser {
private:
    map<set<int>, string> acceptingStates;
    map<set<int>, map<string, set<int>>> transitions;
    set<int> startStates;
    fstream file;
    vector<Token> tokenize(const string &line);

public:
    LanguageParser();
    LanguageParser(string path, map<set<int>, map<string, set<int>>> transitions, map<set<int>, string> acceptingStates, set<int> startStates);
    vector<Token> parseFile();
};
#endif // LANGUAGEPARSER_H
