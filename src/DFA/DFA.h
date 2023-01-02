#ifndef LEXICAL_ANALYZER_DFA_H
#define LEXICAL_ANALYZER_DFA_H

#include <map>
#include <vector>
#include <climits>

#include "../Utils/RegularExpression.h"
#include "../NFA/NFA.h"

class DFA {
public:
    explicit DFA(const vector<RegularExpression> &regExps);

    struct State {
        explicit State(int id) : id(id), isAccepting(false) {
            transitions.resize(CHAR_MAX);
        }

        int id;
        bool isAccepting;
        string regExp;
        vector<int> transitions;
    };

    const vector<State> &getStates() const;

private:

    vector<State> states;

    void minimizeDFA();

    vector<int> classify();

    void reClassify(vector<int> &statesClasses);

    vector<int> renewTransitions(const vector<int> &, const vector<int> &);

    static void markAcceptingStates(State &, const NFA::Set &, const vector<RegularExpression> &);

};


#endif //LEXICAL_ANALYZER_DFA_H