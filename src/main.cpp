#include <iostream>
#include <string>
#include "Parser/LexicalParser.h"
#include "SyntaxParser/LL1GrammarBuilder.h"
#include "SyntaxParser/SyntaxParser.h"
#include "Parser/GrammarParser.h"

using namespace std;


int main(int argc, char *argv[]) {
    string rulesPath = (argc > 2) ? argv[2] : "../grammar.txt";
    string programPath = (argc > 3) ? argv[3] : "../input.txt";
    string outputPath = (argc > 4) ? argv[4] : "../output.txt";
    string cfgPath = (argc > 5) ? argv[5] : "../cfg.txt";
    string ll1GrammarPath = (argc > 6) ? argv[6] : "../ll1.txt";

    LexicalParser lexicalParser(rulesPath);

    if (lexicalParser.hasGrammarError()) {
        cerr << "Error: Grammar File Parsing Error" << "\n";
        return -1;
    }

    lexicalParser.setInputStream(programPath);

    LL1GrammarBuilder builder{cfgPath};
    if (builder.fail()) {
        cerr << "Failed while processing Context free grammar file.\n";
        return -1;
    }
    builder.buildLL1Grammar();
    builder.printLL1Grammar(ll1GrammarPath);

    SyntaxParser syn_parser{builder.getRules(), builder.getStartSymbol()};


    Token token;
    ofstream outputFile{outputPath};
    if (!outputFile.is_open()) {
        cerr << "Error: Output File Creation Error." << "\n";
        return -1;
    }
    auto [derivation, status] = syn_parser.parse(lexicalParser);
    array<string, 3> status_to_string{"Accepted", "Accepted with errors", "Not matched"};
    outputFile << "Syntax parser status: " << status_to_string[static_cast<int>(status)] << "\n";
    for (const vector<Symbol> &cur: derivation) {
        for (const Symbol &symbol: cur) {
            outputFile << ((symbol.type == Symbol::Type::TERMINAL) ? ("'" + symbol.name + "'") : symbol.name);
            outputFile << " ";
        }
        outputFile << "\n";
    }
    outputFile.flush();
    outputFile.close();

//    while (lexicalParser.getNextToken(token)) {
//        outputFile << token.regExp << "\n";
//    }
//    outputFile.flush();
//    outputFile.close();
//
//    lexicalParser.writeTransitionTable("../Transition_Table.txt");
//    lexicalParser.writeAcceptingStates("../Accepting_States.txt");


    return 0;
}
