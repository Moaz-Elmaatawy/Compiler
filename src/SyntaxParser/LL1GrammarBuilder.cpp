#include <algorithm>
#include <cassert>
#include "LL1GrammarBuilder.h"

const char PRODUCTION_SEPARATOR = '|';
const char RULE_START = '#';
const char RULE_ASSIGNMENT = '=';
const char DASH = '\'';

LL1GrammarBuilder::LL1GrammarBuilder(const string &inputFilePath) : hasError(false) {
    fstream file;
    file.open(inputFilePath, ios::in);
    if (!file.is_open()) {
        cerr << "Error opening Context Free Grammar file\n";
        exit(-1);
    }

    string line, nextLine;
    getline(file, line);
    while (getline(file, nextLine)) {
        if (nextLine[0] == RULE_START) {
            insertProductionRule(line);
            line = nextLine;
        } else {
            line += ' ' + nextLine;
        }
    }
    insertProductionRule(line);
    file.close();

    for (const auto &[symbol, defined]: encounteredSymbols) {
        if (!defined) {
            cerr << "Error, No production rules for this symbol: " << symbol << "\n";
            hasError = true;
        }
    }
}

void LL1GrammarBuilder::buildLL1Grammar() {
    eliminateLeftRecursion();
    printLL1Grammar("../NoLeftRec.txt");
    applyLeftFactoring();
}

void LL1GrammarBuilder::printLL1Grammar(const string &ll1GrammarPath) {
    ofstream ll1GrammarFile{ll1GrammarPath};
    for (const auto &[symbol, productions]: rules) {
        ll1GrammarFile << symbol.name << " = ";
        for (auto it = productions.begin(); it != productions.end(); ++it) {
            for (const auto &sym: *it) {
                if (sym.type == Symbol::Type::TERMINAL)
                    ll1GrammarFile << "\'" << sym.name << "\' ";
                else
                    ll1GrammarFile << sym.name << " ";
            }
            if (std::next(it) != productions.end())
                ll1GrammarFile << "\n\t\t| ";
        }
        ll1GrammarFile << "\n\n";
    }
    ll1GrammarFile.flush();
    ll1GrammarFile.close();
}

const unordered_map<Symbol, Rule> &LL1GrammarBuilder::getRules() const {
    return this->rules;
}

const Symbol &LL1GrammarBuilder::getStartSymbol() const {
    return this->startSymbol;
}

bool LL1GrammarBuilder::fail() const {
    return this->hasError;
}

void
LL1GrammarBuilder::insertProductionRule(string &ruleDef) {
    istringstream iss(ruleDef);
    string token, LHS;
    iss >> token;
    if (token.size() != 1 || token[0] != RULE_START) {
        cerr << "Production rules should start with #\n";
        hasError = true;
        return;
    }
    iss >> LHS >> token;
    if (token.size() != 1 || token[0] != RULE_ASSIGNMENT) {
        cerr << "Production rules be on the form LHS " << RULE_ASSIGNMENT << " RHS\n";
        hasError = true;
        return;
    }
    encounteredSymbols[LHS] = true;
    Production currentProd;
    vector<Production> productions;
    while (iss >> token) {
        if (token.size() == 1 && token[0] == PRODUCTION_SEPARATOR) {
            if (currentProd.empty()) {
                cerr << "Production's RHS can't be empty\n";
                hasError = true;
                continue;
            }
            productions.push_back(move(currentProd)), currentProd = Production{};
        } else {
            Symbol symbol;
            if (isTerminal(token)) {
                symbol = {token.substr(1, token.size() - 2), Symbol::Type::TERMINAL};
            } else if (isEpsilon(token)) {
                symbol = epsSymbol;
            } else {
                symbol = {token, Symbol::Type::NON_TERMINAL};
                if (!encounteredSymbols.count(token)) {
                    encounteredSymbols.insert({token, false});
                }
            }
            currentProd.push_back(move(symbol));
        }
    }
    if (currentProd.empty()) {
        cerr << "Productions can't be empty\n";
        hasError = true;
    } else {
        productions.push_back(move(currentProd));
    }

    if (startSymbol.name.empty()) {
        startSymbol = {LHS, Symbol::Type::NON_TERMINAL};
    }
    Symbol lhsSym{LHS, Symbol::Type::NON_TERMINAL};
    Rule &ruleLhs = rules.insert({lhsSym, Rule(lhsSym.name)}).first->second;
    ruleLhs.insert(ruleLhs.end(), productions.begin(), productions.end());
}

bool LL1GrammarBuilder::isTerminal(string &token) {
    return (token[0] == '\'' && token[token.size() - 1] == '\'');
}

bool LL1GrammarBuilder::isEpsilon(string &token) {
    return (token.size() == 2 && token[0] == '\\' && token[1] == 'L');
}

void LL1GrammarBuilder::eliminateLeftRecursion() {
    vector<Symbol> nonTerminalsLHS;
    for (const auto &[symbol, rule]: rules) {
        nonTerminalsLHS.push_back(symbol);
    }
    for (const auto &lhs: nonTerminalsLHS) {
        toImmediateLeftRecursion(lhs, nonTerminalsLHS);
        eliminateImmediateLeftRecursion(rules.at(lhs));
    }
}

void LL1GrammarBuilder::toImmediateLeftRecursion(Symbol lhs, vector<Symbol> nonTerminalsLHS) {
    for (const auto &otherLhs: nonTerminalsLHS) {
        if (lhs == otherLhs) {
            break;
        }
        Rule expandedRule(lhs.name);
        for (auto &prod: rules.at(lhs)) {
            if (prod.front() == otherLhs) {
                auto substituted = expand(prod, rules.at(otherLhs));
                expandedRule.insert(expandedRule.end(), substituted.begin(), substituted.end());
            } else {
                expandedRule.push_back(prod);
            }
        }
        rules.at(lhs) = expandedRule;
    }
}


void LL1GrammarBuilder::eliminateImmediateLeftRecursion(Rule &rule) {
    Rule hasLeftRecursion{""};
    Rule noLeftRecursion{""};
    for (const auto &prod: rule) {
        if (prod.front() == rule.getLhs()) {
            hasLeftRecursion.push_back(prod);
        } else {
            noLeftRecursion.push_back(prod);
        }
    }
    if (hasLeftRecursion.empty()) {
        return;
    }
    if (noLeftRecursion.empty()) {
        cerr << "Error: Invalid Production Rule detected while eliminating left recursion\n";
        exit(-1);
    }
    rule.clear();
    Symbol newLhs = {rule.getLhs().name + DASH, Symbol::Type::NON_TERMINAL};

    for (auto &prod: noLeftRecursion) {
        prod.push_back(newLhs);
        rule.push_back(prod);
    }

    Rule newRule{newLhs.name};
    for (auto &prod: hasLeftRecursion) {
        prod.erase(prod.begin());
        prod.push_back(newLhs);
        newRule.push_back(prod);
    }
    newRule.push_back({epsSymbol});

    rules.insert({newLhs, newRule});
}

Rule LL1GrammarBuilder::expand(Production &curProd, Rule expandedRule) {
    curProd.erase(curProd.begin());
    for (auto &rule: expandedRule) {
        rule.insert(rule.end(), curProd.begin(), curProd.end());
    }
    return expandedRule;
}

void LL1GrammarBuilder::applyLeftFactoring() {
    unordered_map<Symbol, Rule> newRules;
    for (const auto &rule: this->rules) {
        const unordered_map<Symbol, Rule> &factoredRules = leftFactorRule(rule.first, rule.second);
        newRules.insert(factoredRules.begin(), factoredRules.end());
    }
    this->rules = move(newRules);
}

unordered_map<Symbol, Rule> LL1GrammarBuilder::leftFactorRule(const Symbol &lhs, const Rule &rule) {
    unique_ptr<Node> root = make_unique<Node>();
    for (const Production &production: rule)
        addProductionToTrie(root.get(), production);


    unordered_map<Symbol, Rule> newRules;
    if (root->children.size() == 1) {
        // This a special case when the root contains one child.
        // Example: A = BC | BD | BE
        newRules.insert({lhs, Rule(lhs.name)});
        Production newProduction = dfs(root.get(), newRules, lhs);
        reformatProduction(newProduction);
        newRules.at(lhs) = Rule(lhs.name, {move(newProduction)});
    } else {
        dfs(root.get(), newRules, lhs);
    }
    return newRules;
}


void LL1GrammarBuilder::addProductionToTrie(Node *node, const Production &production) {
    for (const auto &symbol: production) {
        if (node->children.find(symbol) == node->children.end())
            node->children.insert({symbol, make_unique<Node>()});
        node = node->children[symbol].get();
    }
    // to mark leaf nodes an epsilon symbol is added at the end
    node->children.insert({epsSymbol, make_unique<Node>()});
}

vector<Symbol> LL1GrammarBuilder::dfs(Node *node, unordered_map<Symbol, Rule> &newRules,
                                      const Symbol &originLhs) {
    if (node->children.empty())
        return {};

    // if node has only one child no need to add new rule.
    if (node->children.size() == 1) {
        const auto &onlyChild = node->children.begin();
        vector<Symbol> symbols = dfs(onlyChild->second.get(), newRules, originLhs);
        symbols.push_back(onlyChild->first);
        return symbols;
    }

    // Generate a name for new rule using the current rules size
    Symbol newLhs = originLhs;
    if (newRules.size() >= 1)
        newLhs.name += to_string(newRules.size());
    newRules.insert({newLhs, Rule(newLhs.name)});


    Rule newRule = Rule(newLhs.name);
    for (const auto &[symbol, child]: node->children) {
        // Get the rest of the production for this child.
        Production newProduction = dfs(child.get(), newRules, originLhs);
        // Add child symbol to be a prefix for the production when reversed.
        newProduction.push_back(symbol);
        reformatProduction(newProduction);
        newRule.emplace_back(move(newProduction));
    }
    newRules.at(newLhs) = Rule(newLhs.name, move(newRule));
    return {newLhs};
}

void LL1GrammarBuilder::reformatProduction(Production &production) {
    reverse(production.begin(), production.end());
    // Unnecessary epsilon was marking leaf node
    if (production.size() >= 2 && production.back() == epsSymbol)
        production.pop_back();
}
