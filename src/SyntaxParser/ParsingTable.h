#ifndef LEXICAL_ANALYZER_PARSINGTABLE_H
#define LEXICAL_ANALYZER_PARSINGTABLE_H


#include "SyntaxUtils.h"

using namespace std;

class ParsingTable {
public:
    ParsingTable(const unordered_map<Symbol, Rule> &rules,
                 const SyntaxUtils &syntaxUtils);

    const Production &getProduction(const Symbol &nonTerminal, const Symbol &terminal) const;

    bool hasProduction(const Symbol &nonTerminal, const Symbol &terminal) const;

    bool fail() const;

    void writeToCSV(const string &fileName);

private:
    unordered_map<Symbol, unordered_map<Symbol, Production>> table;
    bool has_error{};

    void
    addRowToTable(const Symbol &nonTerminal, const vector<Production> &productions, const SyntaxUtils &syntaxUtils);

    void addProductionToRow(const Symbol &nonTerminal, const Symbol &terminal, const Production &production);

    void writeRowToCSV(const vector<string> &row, ofstream &tableFile);

    string toString(const Production &production);
};


#endif
