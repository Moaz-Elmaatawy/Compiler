#include <iostream>
#include "InputLanguageParser.h"

InputLanguageParser::InputLanguageParser() = default;

InputLanguageParser::InputLanguageParser(string filePath, map<set<int>, map<string, set<int>>> transitionsIn, map<set<int>, string> acceptedStatesIn, set<int> startStatesIn) {
    transitions = transitionsIn;
    acceptingStates = acceptedStatesIn;
    startStates = startStatesIn;
    file.open(filePath, ios::in);
}

vector<Token> InputLanguageParser::parseFile() {
    vector<Token> tokens;
    string line;
    while (getline(file, line)) {
        vector <Token> lineTokens = tokenize(line);
        for (const Token &t : lineTokens)
            tokens.emplace_back(t);
    }
    return tokens;
}

vector<Token> InputLanguageParser::tokenize(const string &line) {
    // TODO: Implement according to maximal munch
}

