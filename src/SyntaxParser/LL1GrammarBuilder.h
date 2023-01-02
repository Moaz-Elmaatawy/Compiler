#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <tuple>
#include <memory>
#include "../Utils/ParserUtils.h"
#include "GrammerTypes.h"

#ifndef LEXICAL_ANALYZER_LL1GRAMMARBUILDER_H
#define LEXICAL_ANALYZER_LL1GRAMMARBUILDER_H

#endif


class LL1GrammarBuilder {
public:
    explicit LL1GrammarBuilder(const string &inputFilePath);

    void buildLL1Grammar();

    void printLL1Grammar(const string &ll1GrammarPath);

    const unordered_map<Symbol, Rule> &getRules() const;

    const Symbol &getStartSymbol() const;

    bool fail() const;

private:
    void insertProductionRule(string &ruleDef);

    bool isTerminal(string &token);

    bool isEpsilon(string &token);

    void eliminateLeftRecursion();

    void toImmediateLeftRecursion(Symbol lhs, vector<Symbol> nonTerminalsLHS);

    void eliminateImmediateLeftRecursion(Rule &rule);

    static Rule expand(Production &curProd, Rule expandedRule);

    unordered_map<Symbol, Rule> rules;
    Symbol startSymbol;
    bool has_error;
    unordered_map<string, bool> encounteredSymbols;

    struct Node {
        unordered_map<Symbol, unique_ptr<Node>> children;
    };

    void applyLeftFactoring();

    unordered_map<Symbol, Rule> leftFactorRule(const Symbol &lhs, const Rule &rule);

    void addProduction(Node *node, const Production &production);

    vector<Symbol> dfs(Node *node, unordered_map<Symbol, Rule> &new_rules, const Symbol &origin_lhs);

    void reformat_production(Production &production);
};
