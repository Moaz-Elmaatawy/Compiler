#include "SyntaxUtils.h"
#include <map>
#include <cassert>

SyntaxUtils::SyntaxUtils(const unordered_map<Symbol, Rule> &rules,
                         const Symbol &start_symbol) {

    // First, construct first table.
    for (auto &[non_terminal, _]: rules) {
        this->precompute_first(non_terminal, rules);
        // initialize the follow sets
        this->follow.insert({non_terminal, {}});
    }

    // Then, construct Follow table.
    assert(this->follow.find(start_symbol) != this->follow.end() &&
           "Error: The rules have some undefined symbols, check it again.");
    this->precompute_follow(rules, start_symbol);
}

// Assumption it returns the name of the symbol when the symbol is not non-terminal.
SyntaxUtils::First_set SyntaxUtils::first_of(const Symbol &symbol) const {
    if (symbol.type != Symbol::Type::NON_TERMINAL) {
        return {symbol};
    }
    return SyntaxUtils::find(this->first, symbol);
}

SyntaxUtils::First_set SyntaxUtils::first_of(const Production &production) const {
    First_set curr_first = {epsSymbol};
    for (const Symbol &symbol: production) {
        First_set symbol_first = first_of(symbol);
        curr_first.insert(symbol_first.begin(), symbol_first.end());
        if (symbol_first.find(epsSymbol) == symbol_first.end()) {
            curr_first.erase(epsSymbol);
            break;
        }
    }
    return curr_first;
}

SyntaxUtils::Follow_set SyntaxUtils::follow_of(const Symbol &symbol) const {
    assert(symbol.type == Symbol::Type::NON_TERMINAL && "Error: no follow-set for terminal/epsilon symbol");
    return SyntaxUtils::find(this->follow, symbol);
}

SyntaxUtils::Terminal_set
SyntaxUtils::find(const unordered_map<Symbol, Terminal_set> &set, const Symbol &non_terminal) {
    auto it = set.find(non_terminal);
    if (it == set.end()) {
        return {};
    }
    return it->second;
}

// Assumption: The rules CAN't have Left recursion, it may lead to undefined behavior if there's any.
void SyntaxUtils::precompute_first(const Symbol &non_terminal,
                                   const unordered_map<Symbol, Rule> &rules) {

    // Checks if we have visited that non-terminal before.
    if (this->first.find(non_terminal) != this->first.end()) {
        return;
    }
    this->first.insert({non_terminal, {epsSymbol}});

    // Assumption: no errors of these kinds are allowed.
    assert(rules.find(non_terminal) != rules.end() && "Error: The rules have some undefined symbols, check it again.");

    // Keeps track if the first set of the current non-terminal so far.
    First_set &current_first = this->first.at(non_terminal);

    // As long as epsilon is false, the final first set shouldn't contain epsilon.
    bool add_epsilon = false;
    for (const Production &production: rules.at(non_terminal)) {

        // epsilon flag relative to the current production.
        // it's used also while iterating to decide if we should stop iterating if
        // we encounter a terminal or a non-terminal with a free set without epsilon.
        bool curr_epsilon = true;
        for (int i = 0; i < production.size() && curr_epsilon; i++) {
            const Symbol &symbol = production.at(i);
            if (symbol.type == Symbol::Type::TERMINAL) {
                current_first.insert(symbol);
                curr_epsilon = false;
            } else if (symbol.type == Symbol::Type::NON_TERMINAL) {
                precompute_first(symbol, rules);
                const First_set &temp = this->first.at(symbol);
                if (temp.find(epsSymbol) == temp.end()) {
                    curr_epsilon = false;
                }
                current_first.insert(temp.begin(), temp.end());
            } else if (symbol.type == Symbol::Type::EPSILON) {
                current_first.emplace(epsSymbol);
            }
        }
        add_epsilon |= curr_epsilon;
    }

    // Makes sure if the epsilon should be in the first set or not.
    if (!add_epsilon) {
        current_first.erase(epsSymbol);
    }
}

void SyntaxUtils::precompute_follow(const unordered_map<Symbol, Rule> &rules,
                                    const Symbol &start_symbol) {
    // it's just an assumption to use the dollar sign as an endmarker, following the reference convention.
    const string special_endmarker = "$";
    this->follow.at(start_symbol).insert({special_endmarker, Symbol::Type::TERMINAL});
    this->follow_calculate_by_first(rules);
    this->follow_calculate_by_follow(rules);
}

/*
 *    Performing the calculation of first sets incrementally.
 *    So for example: A => B C D and let's assume D and C have Є
 *    then to calculate FOLLOW(B), we will need to merge FIRST(C) and FIRST(D)
 *    and to calculate FOLLOW(C), we will need to get FIRST(D).
 *    so we can simply starts iterating backward and just merge if the non-terminal has Є.
 */
void SyntaxUtils::follow_calculate_by_first(const unordered_map<Symbol, Rule> &rules) {

    for (const auto &[_, rule]: rules) {
        for (const Production &production: rule) {
            First_set curr_first;
            for (int i = (int) production.size() - 1; i >= 0; i--) {
                const Symbol &symbol = production.at(i);
                if (symbol.type == Symbol::Type::NON_TERMINAL) {
                    // Update the follow set with the first set of the suffix non-terminals.
                    this->follow.at(symbol).insert(curr_first.begin(), curr_first.end());
                    auto symbol_first = this->first.at(symbol);
                    if (symbol_first.find(epsSymbol) == symbol_first.end()) {
                        // reset the first set of the suffix non-terminals.
                        curr_first.clear();
                    } else {
                        symbol_first.erase(epsSymbol);
                    }
                    curr_first.insert(symbol_first.begin(), symbol_first.end());

                } else if (symbol.type == Symbol::Type::TERMINAL) {
                    curr_first.clear();
                    curr_first.insert(symbol);
                }
            }
        }
    }
}

/*
 *  It keeps applying the third rule until no change is observed in the follow sets.
 *  Changes are observed by keeping tracks of the size of each follow set and compare the old with the new one.
 */
void SyntaxUtils::follow_calculate_by_follow(const unordered_map<Symbol, Rule> &rules) {

    bool updated;
    do {
        updated = false;
        for (const auto &[lhs_non_terminal, rule]: rules) {
            const auto &lhs_follow = this->follow.at(lhs_non_terminal);
            for (const Production &production: rule) {
                for (int i = (int) production.size() - 1;
                     i >= 0 && production.at(i).type == Symbol::Type::NON_TERMINAL; i--) {
                    const Symbol &symbol = production.at(i);
                    const auto &symbol_first = this->first.at(symbol);
                    auto &symbol_follow = this->follow.at(symbol);
                    updated |= SyntaxUtils::insert_into(symbol_follow, lhs_follow);
                    if (symbol_first.find(epsSymbol) == symbol_first.end()) {
                        break;
                    }
                }
            }
        }
    } while (updated);
}

bool SyntaxUtils::insert_into(SyntaxUtils::Terminal_set &dest, const SyntaxUtils::Terminal_set &src) {
    int size = dest.size();
    dest.insert(src.begin(), src.end());
    return dest.size() != size;
}
