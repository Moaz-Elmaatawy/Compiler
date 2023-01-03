#include <iostream>
#include <fstream>
#include "ParsingTable.h"

ParsingTable::ParsingTable(const unordered_map<Symbol, Rule> &rules,
                           const SyntaxUtils &syntaxUtils) {

    for (const auto &[nonTerminal, productions]: rules)
        addRowToTable(nonTerminal, productions, syntaxUtils);

    writeToCSV("../Table.csv");
}

bool ParsingTable::hasProduction(const Symbol &nonTerminal, const Symbol &terminal) const {
    if (table.find(nonTerminal) != table.end()) {
        const unordered_map<Symbol, Production> &row = table.at(nonTerminal);
        return row.find(terminal) != row.end();
    }
    return false;
}

const Production &ParsingTable::getProduction(const Symbol &nonTerminal, const Symbol &terminal) const {
    return table.at(nonTerminal).at(terminal);
}


void ParsingTable::addRowToTable(const Symbol &nonTerminal, const vector<Production> &productions,
                                 const SyntaxUtils &syntaxUtils) {

    // store which production to use for the follow set of this non terminal
    Production followProduction = {};
    for (const auto &production: productions) {
        unordered_set<Symbol> productionFirst = syntaxUtils.first_of(production);
        if (productionFirst.count(epsSymbol)) {
            if (followProduction.empty()) {
                followProduction = production;
            } else {
                cerr << "More than one production for non_terminal = " << nonTerminal.name
                     << " evaluates to epsilon.\n";
                has_error = true;
            }
            productionFirst.erase(epsSymbol);
        }
        for (auto &terminal: productionFirst) {
            addProductionToRow(nonTerminal, terminal, production);
        }
    }

    if (followProduction.empty())
        followProduction = SYNC_PRODUCTION;

    unordered_set<Symbol> nonTerminalFollow = syntaxUtils.follow_of(nonTerminal);
    for (auto &terminal: nonTerminalFollow) {
        addProductionToRow(nonTerminal, terminal, followProduction);
    }

}

void ParsingTable::addProductionToRow(const Symbol &nonTerminal, const Symbol &terminal, const Production &production) {
    unordered_map<Symbol, Production> &row = table[nonTerminal];
    if (row.find(terminal) == row.end()) {
        row[terminal] = production;
    } else if (production != SYNC_PRODUCTION) {
        cerr << "More than one production at entry of non_terminal = " << nonTerminal.name << " and terminal = "
             << terminal.name << " .\n";
        has_error = true;
    }
}

bool ParsingTable::fail() const {
    return has_error;
}

void ParsingTable::writeToCSV(const string &fileName) {
    ofstream tableFile;
    tableFile.open(fileName);

    unordered_set<Symbol> terminals;
    for (const auto &[symbol, row]: this->table) {
        for (const auto &[terminal, production]: row)
            terminals.insert(terminal);
    }

    vector<string> firstRow;
    firstRow.reserve(terminals.size() + 1);
    firstRow.push_back("");
    for (const auto &symbol: terminals)
        firstRow.push_back("'" + symbol.name + "'");
    writeRowToCSV(firstRow, tableFile);

    for (const auto &[nonTerminal, row]: this->table) {
        vector<string> fileRow;
        fileRow.reserve(terminals.size() + 1);
        fileRow.push_back(nonTerminal.name);
        for (const auto &terminal: terminals) {
            if (hasProduction(nonTerminal, terminal))
                fileRow.push_back(toString(getProduction(nonTerminal, terminal)));
            else
                fileRow.push_back("");
        }
        writeRowToCSV(fileRow, tableFile);
    }
    tableFile.close();
}

void ParsingTable::writeRowToCSV(const vector<string> &row, ofstream &tableFile) {
    for (const auto &word: row)
        tableFile << " " << word << ",";
    tableFile << "\n";
}

string ParsingTable::toString(const Production &production) {
    if (production == SYNC_PRODUCTION)
        return "SYNC";

    string text;
    for (const auto &symbol: production) {
        if (symbol.type == Symbol::Type::TERMINAL)
            text += ("'" + symbol.name + "' ");
        else
            text += (symbol.name + " ");
    }
    return text;
}
