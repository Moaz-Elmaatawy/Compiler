#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

#ifndef DFA_H
#define DFA_H

class DFA {
private:
    map<int, map<string, vector<int>>> NFATransitions;
    map<int, string> NFAacceptingStates;
    set<string> alphabet;
    map<string, int> regexPriority;

    map<set<int>, string> acceptingStates;
    map<set<int>, map<string, set<int>>> transitions;
    set<int> startStates;

    bool contains(vector<set<int>> *pVector, set<int> set);
    void getEpsilonClosure(set<int> *from);
    void explore(set<int> *from, string input, set<int> *to);

public:
    DFA();
    DFA(map<int, map<string, vector<int>>> NFATransitions, set<string> alphabet, map<int, string> NFAacceptingStates, map<string, int> regexPriority);
    void createDFA();

    map<set<int>, map<string, set<int>>> getTransitions();
    map<set<int>, string> getAcceptingStates();
    set<int> getStartStates();
    void stringifyDFA(map<string, map<string, string>> *nwG, map<string, string> *nwAcc, string *nwS);
};

#endif // DFA_H
