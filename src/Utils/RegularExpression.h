#ifndef LEXICAL_ANALYZER_REGULAREXPRESSION_H
#define LEXICAL_ANALYZER_REGULAREXPRESSION_H

#include <utility>

#include "string"
#include "../NFA/NFA.h"

class RegularExpression {
private:
    string name;
    int priority;
    NFA nfa;

public:
    RegularExpression(const string &name, int priority, const NFA &nfa) : name(name), priority(priority), nfa(nfa) {}

    const NFA &getNFA() const {
        return nfa;
    }

    const string &getName() const {
        return name;
    }

    const int getPriority() const {
        return priority;
    }
};

#endif //LEXICAL_ANALYZER_REGULAREXPRESSION_H
