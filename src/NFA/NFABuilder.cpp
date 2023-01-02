#include "NFABuilder.h"

NFABuilder::NFABuilder() {

}

NFABuilder::NFABuilder(const NFA &nfa) : nfa(nfa) {

}

NFABuilder &NFABuilder::Concatenate(NFA rhs) {
    nfa.end->addTransition(EPSILON, move(rhs.start));
    nfa.end = move(rhs.end);
    return *this;
}

NFABuilder &NFABuilder::Concatenate(const char c) {
    return Concatenate(NFA{c});
}

NFABuilder &NFABuilder::Or(NFA rhs) {
    auto new_start{make_shared<NFA::Node>()};
    auto new_end{make_shared<NFA::Node>()};

    new_start->addTransition(EPSILON, move(nfa.start));
    new_start->addTransition(EPSILON, move(rhs.start));
    nfa.end->addTransition(EPSILON, new_end);
    rhs.end->addTransition(EPSILON, new_end);
    nfa.start = move(new_start);
    nfa.end = move(new_end);
    return *this;
}

NFABuilder &NFABuilder::Or(const char c) {
    return Or(NFA{c});
}

NFABuilder &NFABuilder::PositiveClosure() {
    auto new_start{make_shared<NFA::Node>()};
    auto new_end{make_shared<NFA::Node>()};

    nfa.end->addTransition(EPSILON, nfa.start);
    new_start->addTransition(EPSILON, move(nfa.start));
    nfa.end->addTransition(EPSILON, new_end);
    nfa.start = move(new_start);
    nfa.end = move(new_end);
    return *this;
}

NFABuilder &NFABuilder::KleeneClosure() {
    this->PositiveClosure();
    nfa.start->addTransition(EPSILON, nfa.end);
    return *this;
}

NFA NFABuilder::build() {
    return move(nfa);
}