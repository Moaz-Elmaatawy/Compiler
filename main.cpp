#include <iostream>
#include "InputParser.h"
#include "LanguageParser.h"
#include "DFA.h"
#include "Nfa.h"
#include "MinimizedDfa.h"
#include "Token.h"
#include "FileWriter.h"

using namespace std;

int main(int argc, char** argv){
    string grammarPath, inputPath, outputPath;
    list<string> temp;
    grammarPath = (argc > 2) ? argv[2] : "grammar.txt";
    inputPath = (argc > 3) ? argv[3] : "input.txt";
    outputPath = (argc > 4) ? argv[4] : "result.txt";

    InputParser inputParser(grammarPath);
    inputParser.readFile();
    map<string, string> regs = inputParser.getRegexes();
    for(auto i : inputParser.getKeywords()){
        regs[i] = i;
    }
    for(auto i : inputParser.getPunctuations()){
        regs[i] = "\\"+i;
    }

    Nfa nfa = Nfa();
    nfa = nfa.getfromlist(regs);

    DFA dfa(nfa.transitions, nfa.getAlphabets(), nfa.tags, inputParser.getRegexPriority());
    dfa.createDFA();
    map<set<int>, map<string, set<int>>> dfaGraph = dfa.getTransitions();
    map<set<int>, string> dfaAccepted = dfa.getAcceptingStates();
    set<int> start = dfa.getStartStates();


    minimizeDfa(&dfaGraph, &dfaAccepted, nfa.getAlphabets(), &start);
    map<string, map<string, string>> dfaForm;
    map<string, string> accForm;
    string startState;
    dfa.stringifyDFA(&dfaForm, &accForm, &startState);

    LanguageParser langParser(inputPath, dfaGraph, dfaAccepted, start);
    vector<Token> tokens = langParser.parseFile();

    FileWriter fileWriter;
    fileWriter.printTransitionTable(dfaForm);
    fileWriter.printAcceptingStates(accForm);
    fileWriter.printResult(outputPath, tokens);

    return 0;
}