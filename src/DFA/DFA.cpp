#include "DFA.h"

const vector<DFA::State> &DFA::getStates() const {
    return states;
}

DFA::DFA(const vector<RegularExpression> &regExps) {
    queue<NFA::Set> frontier;
    map<NFA::Set, int> visited;
    NFA::Set start;
    for (const auto &regEXP: regExps) {
        start.insert(regEXP.getNFA().getStart());
    }
    start = getEpsilonClosure(start);
    int stateID = 0;
    visited[start] = stateID;
    states.emplace_back(stateID++);
    frontier.push(start);
    while (!frontier.empty()) {
        auto current = move(frontier.front());
        frontier.pop();
        int index = visited.at(current);
        markAcceptingStates(states[index], current, regExps);
        for (char c = 1; c < CHAR_MAX; ++c) {
            NFA::Set next = getEpsilonClosure(Move(current, c));
            if (!visited.count(next)) {
                visited.insert({next, stateID});
                states.emplace_back(stateID++);
                frontier.push(next);
            }
            states[index].transitions[c] = visited.at(next);
        }
    }
    int emptySetIndex = visited.at(NFA::Set());
    for (auto &state: states) {
        state.transitions[0] = emptySetIndex;
    }
    this->minimizeDFA();
}

void DFA::markAcceptingStates(DFA::State &state, const NFA::Set &set, const vector<RegularExpression> &regEXPs) {
    int priority = INT_MAX;
    for (const auto &regEXP: regEXPs) {
        if (!set.count(regEXP.getNFA().getEnd())) {
            continue;
        }
        if (regEXP.getPriority() < priority) {
            priority = regEXP.getPriority();
            state.regExp = regEXP.getName();
            state.isAccepting = true;
        }
    }
}

void DFA::minimizeDFA() {
    vector<int> statesClasses = classify();
    vector<State> newStates;
    for (int i = 0; i < states.size(); i++) {
        if (statesClasses[i] == newStates.size()) {
            newStates.emplace_back(move(states[i]));
            newStates.back().id = statesClasses[i];
            newStates.back().transitions = renewTransitions(newStates.back().transitions, statesClasses);
        }
    }
    states = move(newStates);
}

vector<int> DFA::classify() {
    int nextClass = 1;
    unordered_map<string, int> regExpClass;
    vector<int> statesClasses(states.size());

    for (int i = 0; i < states.size(); i++) {
        if (states[i].isAccepting) {
            if (regExpClass.find(states[i].regExp) == regExpClass.end()) {
                regExpClass[states[i].regExp] = nextClass;
                nextClass++;
            }
            statesClasses[i] = regExpClass[states[i].regExp];
        } else {
            statesClasses[i] = 0;
        }
    }

    reClassify(statesClasses);
    return statesClasses;
}

void DFA::reClassify(vector<int> &statesClasses) {
    vector<int> newStatesClasses(states.size());
    do {
        int nextClass = 0;
        map<pair<vector<int>, int>, int> classes;

        for (int i = 0; i < states.size(); i++) {
            pair<vector<int>, int> key = {
                    renewTransitions(states[i].transitions, statesClasses),
                    statesClasses[i]
            };
            if (classes.count(key) == 0) {
                classes[key] = nextClass;
                nextClass++;
            }
            newStatesClasses[i] = classes[key];
        }

        swap(statesClasses, newStatesClasses);
    } while (statesClasses != newStatesClasses);
}

vector<int> DFA::renewTransitions(const vector<int> &transitions, const vector<int> &statesClasses) {
    vector<int> transitionClass(CHAR_MAX);
    for (char c = 0; c < CHAR_MAX; c++)
        transitionClass[c] = statesClasses[transitions[c]];
    return transitionClass;
}