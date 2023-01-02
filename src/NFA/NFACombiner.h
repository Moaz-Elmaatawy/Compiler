#ifndef LEXICAL_ANALYZER_NFACOMBINER_H
#define LEXICAL_ANALYZER_NFACOMBINER_H

#include <string>
#include "../Utils/Component.h"
#include "NFABuilder.h"

using namespace std;

class NFACombiner {
private:
    unordered_map<string, NFA> regToNFA;

    NFABuilder getBetweenBrackets(const vector<component> &, int *);

    static NFA charToNFA(const component &);

    static NFABuilder applyToOperation(const component &, const component &);

    static NFABuilder applyClosure(const component &comp, NFABuilder nfaBuilder);

    static NFABuilder applyBinaryOperation(componentType, NFABuilder, NFABuilder);

public:

    const unordered_map<string, NFA> &
    regDefsToNFAs(const vector<pair<string, vector<component>>> &);


    NFA regDefToNFA(const vector<component> &components);

};


#endif //LEXICAL_ANALYZER_NFACOMBINER_H
