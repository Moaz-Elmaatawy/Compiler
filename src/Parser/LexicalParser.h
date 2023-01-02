#ifndef LEXICAL_ANALYZER_LEXICALPARSER_H
#define LEXICAL_ANALYZER_LEXICALPARSER_H

#include <string>
#include <fstream>
#include <iomanip>
#include "../Utils/RegularExpression.h"
#include "../DFA/DFA.h"

using namespace std;

struct Token {
    string regExp;
    string matchString;
};

class LexicalParser {
public:

    explicit LexicalParser(const string &);

    bool getNextToken(Token &);

    void setInputStream(const string &);

    bool hasGrammarError() const;

    void writeTransitionTable(const string&);

    void writeAcceptingStates(const string&);

        private:
    fstream fileStream;
    queue<Token> tokenQueue;
    int lineNumber{};
    const DFA dfa;
    bool errorEncountered;

    int getNextLine();

    void applyMaximalMunch(const string &);

    vector<RegularExpression> getRegExps(const string&);

    DFA getDFA(const string &);

    };

#endif //LEXICAL_ANALYZER_LEXICALPARSER_H
