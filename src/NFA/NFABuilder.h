#ifndef LEXICAL_ANALYZER_NFABUILDER_H
#define LEXICAL_ANALYZER_NFABUILDER_H

#include "NFA.h"

class NFABuilder {
public:

    NFABuilder();


    explicit NFABuilder(const NFA &Nfa);

    NFABuilder &Concatenate(NFA rhs);

    NFABuilder &Concatenate(char c);

    NFABuilder &Or(NFA rhs);

    NFABuilder &Or(char c);

    NFABuilder &PositiveClosure();

    NFABuilder &KleeneClosure();

    NFA build();

private:
    NFA nfa;
};


#endif //LEXICAL_ANALYZER_NFABUILDER_H
