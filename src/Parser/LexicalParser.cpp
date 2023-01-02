#include <iterator>
#include <sstream>
#include <iostream>

#include "LexicalParser.h"
#include "GrammarParser.h"
#include "../NFA/NFACombiner.h"

LexicalParser::LexicalParser(const string &inputFilePath) : dfa(getDFA(inputFilePath)) {}


DFA LexicalParser::getDFA(const string &inputFilePath) {
    vector<RegularExpression> exps = getRegExps(inputFilePath);
    DFA dfa = DFA(exps);
    return dfa;
}

void LexicalParser::writeTransitionTable(const string& transitionTableFilePath) {
    ofstream transitionsFile(transitionTableFilePath);
    cout<<"Number of States: "<<dfa.getStates().size()<<endl;

    vector<DFA::State> states = dfa.getStates();
    int numberOfStates = states.size();
    for(int i=0;i<numberOfStates;++i){
        string stateString = "S"+to_string(i)+(states[i].isAccepting?(" "+states[i].regExp):"");
        transitionsFile<< setw(12) << left <<stateString;
        for(char j=33;j<126;++j){
            string s=string(1,j)+" ==> "+(states[i].transitions[j]==1?"x":("S"+to_string(states[i].transitions[j])));
            transitionsFile<<setw(20)<<left<<s;
        }
        transitionsFile<<endl;
    }
}

void LexicalParser::writeAcceptingStates(const string& acceptingStatesFilePath) {
    ofstream acceptingStatesFile(acceptingStatesFilePath);

    vector<DFA::State> states = dfa.getStates();
    int numberOfStates = states.size();
    int acceptingStatesCount = 0;
    for(int i=0;i<numberOfStates;++i){
        if(states[i].isAccepting) {
            string stateString = "S"+to_string(i) + "\t" + states[i].regExp;
            acceptingStatesFile << stateString << endl;
            acceptingStatesCount++;
        }
    }
    cout<<"Number of Accepting States: "<<acceptingStatesCount<<endl;
}

void LexicalParser::setInputStream(const string &input_stream) {
    this->fileStream.open(input_stream, ios::in);
    this->lineNumber = 0;
}


bool LexicalParser::hasGrammarError() const {
    return this->errorEncountered;
}


bool LexicalParser::getNextToken(Token &token) {
    if (tokenQueue.empty() && !getNextLine()) {
        return false;
    }
    token = move(tokenQueue.front());
    tokenQueue.pop();
    return true;
}

int LexicalParser::getNextLine() {
    if (!this->fileStream.is_open()) {
        return 0;
    }
    string line;
    if (getline(this->fileStream, line)) {
        lineNumber++;
        istringstream iss(line);
        vector<string> words{istream_iterator<string>{iss},
                                       istream_iterator<string>{}};
        for (const auto &word: words) {
            applyMaximalMunch(word);
        }
    }
    return this->tokenQueue.size();
}

void LexicalParser::applyMaximalMunch(const string &segment) {
    if (segment.empty()) return;

    int lastAcceptingState = -1;
    int lastAcceptingStateIndex = -1;
    int i = 0;

    const vector<DFA::State> &states = this->dfa.getStates();
    while (i < segment.length()) {
        const DFA::State *state = &states.at(0);
        for (int j = i; j < segment.length(); j++) {
            state = &states.at(state->transitions.at(segment[j]));
            if (state->isAccepting) {
                lastAcceptingStateIndex = j;
                lastAcceptingState = state->id;
            }
        }
        if (lastAcceptingStateIndex < i) {
            cerr << "Error in line " << lineNumber << " :" << segment.substr(i) << " Couldn't match\n";
            i++;
            continue;
        }
        this->tokenQueue.push({states.at(lastAcceptingState).regExp,
                               segment.substr(i, lastAcceptingStateIndex - i + 1)});
        i = lastAcceptingStateIndex + 1;
    }
}

vector<RegularExpression> LexicalParser::getRegExps(const string&inputFilePath) {
    GrammarParser parser = GrammarParser(inputFilePath);
    const vector<pair<string, vector<component>>> &regDefComponents =
            parser.getRegDefComponents();
    vector<string> regExps = parser.getRegExp();

    NFACombiner componentParser;
    const unordered_map<string, NFA> &regDefToNFA = componentParser.regDefsToNFAs(
            regDefComponents);

    this->errorEncountered = false;
    vector<RegularExpression> results;
    int order = 1;
    for (string &regExp: regExps) {
        if (regDefToNFA.find(regExp) != regDefToNFA.end()) {
            results.emplace_back(regExp, order++, regDefToNFA.at(regExp));
        } else {

            this->errorEncountered = true;
        }
    }
    return results;
}


