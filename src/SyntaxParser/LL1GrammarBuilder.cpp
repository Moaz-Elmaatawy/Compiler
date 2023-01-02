#include <algorithm>
#include <cassert>
#include "LL1GrammarBuilder.h"

const char PRODUCTION_SEPARATOR = '|';
const char RULE_START = '#';
const char RULE_ASSIGNMENT = '=';
const char DASH = '\'';

LL1GrammarBuilder::LL1GrammarBuilder(const string &inputFilePath) : has_error(false) {
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
            has_error = true;
        }
    }
}

void LL1GrammarBuilder::buildLL1Grammar() {
    eliminateLeftRecursion();
    applyLeftFactoring();
}

void LL1GrammarBuilder::printLL1Grammar(const string &ll1GrammarPath){
    ofstream ll1GrammarFile{ll1GrammarPath};
    for (const auto &[symbol, productions]: rules) {
        ll1GrammarFile<<symbol.name<<" = ";
        for(auto it = productions.begin(); it != productions.end(); ++it){
            for(const auto &sym : *it){
                if(sym.type == Symbol::Type::TERMINAL)
                    ll1GrammarFile<<"\'"<<sym.name<<"\' ";
                else
                    ll1GrammarFile<<sym.name<<" ";
            }
            if(std::next(it) != productions.end())
                ll1GrammarFile<<" | ";
        }
        ll1GrammarFile<<"\n";
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
    return this->has_error;
}

void
LL1GrammarBuilder::insertProductionRule(string &ruleDef) {
    istringstream iss(ruleDef);
    string token, LHS;
    iss >> token;
    if (token.size() != 1 || token[0] != RULE_START) {
        cerr << "Production rules should start with #\n";
        has_error = true;
        return;
    }
    iss >> LHS >> token;
    if (token.size() != 1 || token[0] != RULE_ASSIGNMENT) {
        cerr << "Production rules be on the form LHS "<<RULE_ASSIGNMENT<<" RHS\n";
        has_error = true;
        return;
    }
    encounteredSymbols[LHS] = true;
    Production current_prod;
    vector<Production> productions;
    while (iss >> token) {
        if (token.size() == 1 && token[0] == PRODUCTION_SEPARATOR) {
            if (current_prod.empty()) {
                cerr << "Production's RHS can't be empty\n";
                has_error = true;
                continue;
            }
            productions.push_back(move(current_prod)), current_prod = Production{};
        } else {
            Symbol symbol;
            if (isTerminal(token)) {
                symbol = {token.substr(1, token.size() - 2), Symbol::Type::TERMINAL};
            } else if (isEpsilon(token)) {
                symbol = eps_symbol;
            } else {
                symbol = {token, Symbol::Type::NON_TERMINAL};
                if (!encounteredSymbols.count(token)) {
                    encounteredSymbols.insert({token, false});
                }
            }
            current_prod.push_back(move(symbol));
        }
    }
    if (current_prod.empty()) {
        cerr << "Productions can't be empty\n";
        has_error = true;
    } else {
        productions.push_back(move(current_prod));
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
    newRule.push_back({eps_symbol});

    rules.insert({newLhs, newRule});
}

Rule LL1GrammarBuilder::expand(Production &curProd, Rule expandedRule) {
    curProd.erase(curProd.begin());
    for (auto &rule: expandedRule) {
        rule.insert(rule.end(), curProd.begin(), curProd.end());
    }
    return expandedRule;
}

/*
 * left factoring is applied on each Rule independently.
 * Each Rule is converted to multiple rules that represent the new rules.
 */
void LL1GrammarBuilder::applyLeftFactoring() {
    unordered_map<Symbol, Rule> new_rules;
    for (const auto &rule: this->rules) {
        const unordered_map<Symbol, Rule> &factored_rules = leftFactorRule(rule.first, rule.second);
        new_rules.insert(factored_rules.begin(), factored_rules.end());
    }
    this->rules = move(new_rules);
}

/*
 * To factor a single Rule.
 * 1- Build a trie for all productions present in the Rule.
 * 2- Traverse the trie and add new rules when needed.
 */
unordered_map<Symbol, Rule> LL1GrammarBuilder::leftFactorRule(const Symbol &lhs, const Rule &rule) {
    unique_ptr<Node> root = make_unique<Node>();
    // building the trie by adding each production symbols to it.
    for (const Production &production: rule)
        addProduction(root.get(), production);

    //this will store the new rules generated from factoring given rule
    unordered_map<Symbol, Rule> new_rules;
    if (root->children.size() == 1) {
        // This a special case when the root contains one child.
        // We must add the first rule here from the return value from dfs call.
        new_rules.insert({lhs, Rule(lhs.name)});
        Production new_production = dfs(root.get(), new_rules, lhs);
        reformat_production(new_production);
        new_rules.at(lhs) = Rule(lhs.name, {move(new_production)});
    } else {
        dfs(root.get(), new_rules, lhs);
    }
    return new_rules;
}

// add a production to the trie by adding its symbols to corresponding nodes.
void LL1GrammarBuilder::addProduction(Node *node, const Production &production) {
    for (const auto &symbol: production) {
        if (node->children.find(symbol) == node->children.end())
            node->children.insert({symbol, make_unique<Node>()});
        node = node->children[symbol].get();
    }
    // add epsilon at the end of every production to mark leaf nodes.
    // handle the case when one production is prefix of another.
    node->children.insert({eps_symbol, make_unique<Node>()});
}

// dfs the whole trie and add a rule for every node with more than one child.
vector<Symbol> LL1GrammarBuilder::dfs(Node *node, unordered_map<Symbol, Rule> &new_rules,
                                      const Symbol &origin_lhs) {
    // base case when reaching leaf node.
    if (node->children.empty())
        return {};

    // if node has only one child no need to add new rule.
    // Just continue the dfs and the child to be a prefix to the returned value when reversed.
    if (node->children.size() == 1) {
        const auto &onlyChild = node->children.begin();
        vector<Symbol> symbols = dfs(onlyChild->second.get(), new_rules, origin_lhs);
        symbols.push_back(onlyChild->first);
        return symbols;
    }

    // if more than one child is present, we need to add a rule where each child will have a production in this rule.
    Symbol new_lhs = origin_lhs;
    //The new Rule lhs is determined from the new_rules current size to have a unique lhs for each rule (A, A1, A2, ...)
    if (new_rules.size() >= 1)
        new_lhs.name += to_string(new_rules.size());
    new_rules.insert({new_lhs, Rule(new_lhs.name)});


    Rule new_rule = Rule(new_lhs.name);
    for (const auto &[symbol, child]: node->children) {
        // Get the rest of the production for this child.
        Production new_production = dfs(child.get(), new_rules, origin_lhs);
        // Add child symbol to be a prefix for the production when reversed.
        new_production.push_back(symbol);
        reformat_production(new_production);
        new_rule.emplace_back(move(new_production));
    }
    new_rules.at(new_lhs) = Rule(new_lhs.name, move(new_rule));
    return {new_lhs};
}

// Reverse the production and Remove the epsilon added to mark leaf nodes if it unnecessary.
// It is kept when one production is prefix of another
// Example : A -> a | ab
// After   : A -> aA1
//          A1 -> b | Є
void LL1GrammarBuilder::reformat_production(Production &production) {
    reverse(production.begin(), production.end());
    if (production.size() >= 2 && production.back() == eps_symbol)
        production.pop_back();
}


