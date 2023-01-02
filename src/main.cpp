#include <iostream>
#include <string>
#include "Parser/LexicalParser.h"

using namespace std;


int main(int argc, char *argv[]) {
    string rulesPath = (argc > 2) ? argv[2] : "../grammar.txt";
    string programPath = (argc > 3) ? argv[3] : "../input.txt";
    string outputPath = (argc > 4) ? argv[4] : "../output.txt";

    LexicalParser lexicalParser(rulesPath);

    if (lexicalParser.hasGrammarError()) {
        cerr << "Error: Grammar File Parsing Error" << "\n";
        return -1;
    }

    lexicalParser.setInputStream(programPath);
    Token token;
    ofstream outputFile{outputPath};
    if (!outputFile.is_open()) {
        cerr << "Error: Output File Creation Error." << "\n";
        return -1;
    }

    while (lexicalParser.getNextToken(token)) {
        outputFile << token.regExp << "\n";
    }
    outputFile.flush();
    outputFile.close();

    lexicalParser.writeTransitionTable("../Transition_Table.txt");
    lexicalParser.writeAcceptingStates("../Accepting_States.txt");


    return 0;
}
