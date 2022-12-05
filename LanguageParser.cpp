#include <iostream>
#include "LanguageParser.h"

LanguageParser::LanguageParser() = default;

LanguageParser::LanguageParser(string filePath, map<set<int>, map<string, set<int>>> transitionsIn, map<set<int>, string> acceptedStatesIn, set<int> startStatesIn) {
    transitions = transitionsIn;
    acceptingStates = acceptedStatesIn;
    startStates = startStatesIn;
    file.open(filePath, ios::in);
}

vector<Token> LanguageParser::parseFile() {
    vector<Token> tokens;
    string line;
    while (getline(file, line)) {
        vector <Token> lineTokens = tokenize(line);
        for (const Token &t : lineTokens)
            tokens.emplace_back(t);
    }
    return tokens;
}

vector<Token> LanguageParser::tokenize(const string &line) {
    vector<Token> tokens;
    long unsigned int prevStart = 0, prevEnd = 0;
    // currType is type based on the last parsed character
    // prevType is the last correct match encountered
    string currType, prevType;
    set<int> currentState = startStates;
    long unsigned int i = 0;
    while (i < line.size()) {
        // does the next char constitute a valid transition? if yes make the transition
        if (transitions[currentState].find(string(1, line.at(i))) != transitions[currentState].end())
            currentState = transitions[currentState][string(1, line.at(i))];
        else {
            // if we have seen a valid match previously, add it to the token vector
            if (currType.empty() && prevStart != i) {
            redo:
                tokens.emplace_back(prevType, line.substr(prevStart, prevEnd - prevStart + 1));
                prevEnd++;
                i = prevEnd;
                prevStart = prevEnd;
                currentState = startStates;
                prevType = "";
                continue;
            }
            // this only happens if the above condition is false, it means that currType is a valid match, so use it
            // note that currType will always == prevType if currType isn't empty
            else
                tokens.emplace_back(prevType, line.substr(prevStart, prevEnd - prevStart + 1));

            // Skip spaces and tabs
            if (string(1, line.at(i)) == " " || string(1, line.at(i)) == "\t") {
                prevStart = i + 1;
                prevEnd = i + 1;
            }
            else {
                prevEnd = i;
                prevStart = i;
                --i;
            }
            currentState = startStates;
            prevType = "";
        }
        // if current state is an accepting state then store it
        if (acceptingStates.find(currentState) != acceptingStates.end() && !acceptingStates[currentState].empty()) {
            currType = acceptingStates[currentState];
            prevType = currType;
            prevEnd = i;
        }
        else
            currType = "";
        ++i;
    }
    currType = acceptingStates[currentState];
    // if after last input, not in a accepting state, go back to prevType and redo maximal munch from there
    if (currType.empty() && !line.empty())
        goto redo;

    if (!line.empty())
        tokens.emplace_back(currType, line.substr(prevStart, prevEnd - prevStart + 1));
    return tokens;
}
