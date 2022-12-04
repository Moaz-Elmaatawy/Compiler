#include "Dfa.h"
#include <iostream>
#include <string>
#include <stdio.h>

Dfa::Dfa() = default;

Dfa::Dfa(map<int, map<string, vector<int>>> NFATransitionsIn, set<string> alphabetIn, map<int, string> NFAacceptingStatesIn, map<string, int> regexPriorityIn) {
    NFATransitions = NFATransitionsIn;
    alphabet = alphabetIn;
    regexPriority = regexPriorityIn;
    NFAacceptingStates = NFAacceptingStatesIn;
}

map<set<int>, map<string, set<int>>> Dfa::getTransitions() {
    return transitions;
}

map<set<int>, string> Dfa::getAcceptingStates() {
    return acceptingStates;
}

set<int> Dfa::getStartStates() {
    return startStates;
}

// Create DFA from NFA transitions
// Assume start state is always state numbered 1 in NFA (Along with its epsilon closure)
void Dfa::createDFA() {
    vector<set<int>> frontier, visited;
    set<int> initial;
    initial.insert(1);
    frontier.push_back(initial);

    getEpsilonClosure(&initial);
    startStates = initial;

    while(!frontier.empty()){
        set<int> current = frontier.at(0);
        set<int> destination;

        frontier.erase(frontier.begin());
        getEpsilonClosure(&current);
        visited.push_back(current);

        // add possible transitions from start using token
        for(string token : alphabet){
            explore(&current, token, &destination);
            getEpsilonClosure(&destination);
            if(!destination.empty() && !contains(&visited, destination) && !contains(&frontier, destination)){
                frontier.push_back(destination);
            }
            transitions[current][token] = destination;
            destination.clear();
        }
        
        // determine if it is an accepting state
        for(auto & NFAacceptingState : NFAacceptingStates){
            if(regexPriority.find(NFAacceptingState.second) != regexPriority.end() && current.find(NFAacceptingState.first) != current.end()){
                // if this isn't the first time this accepting state if encountered priority has to be considered
                if(acceptingStates.find(current) != acceptingStates.end()){
                    if(regexPriority[acceptingStates[current]] > regexPriority[NFAacceptingState.second])
                        acceptingStates[current] = NFAacceptingState.second;
                }
                else{
                    acceptingStates[current] = NFAacceptingState.second;
                }
            }
        }
    }
}

// Get DFA ready for printing to files
void Dfa::stringifyDFA(map<string, map<string, string>> *newTransitions, map<string, string> *newAcceptingStates, string* newStartingState) {
    map<set<int>, string> stringMap;
    int stateNumber = 0;
    // Transform transitions start
    for(const auto& i : transitions){
        string state = "S" + to_string(stateNumber++);
        stringMap[i.first] = state;
    }
    
    // Transform transitions destination
    for(const auto& transition : transitions){
        for(const auto& mapping : transition.second){
            if(mapping.second.size() == 0) 
                (*newTransitions)[stringMap[transition.first]][mapping.first] = "";
            else 
                (*newTransitions)[stringMap[transition.first]][mapping.first] = stringMap[mapping.second];
        }
    }
    // Transform accepting states
    for(auto i : acceptingStates){
        (*newAcceptingStates)[stringMap[i.first]] = i.second;
    }
    // Transform start state
    (*(newStartingState)) = stringMap[startStates];
}


// expands state to contain all states in its epsilon closure set
void Dfa::getEpsilonClosure(set<int> *state) {
    if (state->empty()) return;
    long unsigned int prevSize = state->size();
    do{
        prevSize = state->size();
        set<int> destination;
        explore(state, "$", &destination);
        copy(destination.begin(), destination.end(), inserter(*state, state->begin()));
    } while (prevSize != state->size());
}

// Explores all possible states that can be reached from start using input and adds them all to destination
void Dfa::explore(set<int> *start, string input, set<int> *destination) {
    vector<int> transitionsSet;
    for(int i : *start){
        transitionsSet = NFATransitions[i][input];
        copy(transitionsSet.begin(), transitionsSet.end(), inserter(*destination, destination->begin()));
    }
}

// simple helper to check if set of integers is in given vector of sets
bool Dfa::contains(vector<set<int>> *pVector, set<int> set) {
    for(auto & i : *pVector){
        if(i == set) return true;
    }
    return false;
}
