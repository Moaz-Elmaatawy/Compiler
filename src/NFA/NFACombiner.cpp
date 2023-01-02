#include <stack>
#include <iostream>
#include "NFACombiner.h"


template<typename T>
T poll(stack<T> &st) {
    if (!st.empty()) {
        T top = move(st.top());
        st.pop();
        return top;
    }
    throw logic_error("Stack is empty, Format error.");
}

const unordered_map<string, NFA> &
NFACombiner::regDefsToNFAs(const vector<pair<string, vector<component>>> &regDefinitions) {
    for (const auto &[regularDefinition, components]: regDefinitions) {
        try {
            if (components.size() == 1 && components[0].regularDefinition.size() == 1)
                this->regToNFA[regularDefinition] = NFACombiner::charToNFA(components[0]);
            else
                this->regToNFA[regularDefinition] = this->regDefToNFA(components);
        } catch (logic_error &e) {
            cerr << "Couldn't Parse " << regularDefinition << ", " << e.what() << "\n";
        }
    }
    return this->regToNFA;
}

NFA NFACombiner::charToNFA(const component &regDefinitionChar) {
    return NFA(regDefinitionChar.regularDefinition[0]);
}

NFA NFACombiner::regDefToNFA(const vector<component> &components) {

    stack<NFABuilder> NFABuilders;
    stack<componentType> operations;

    for (int i = 0; i < components.size(); i++) {
        component comp = components[i];

        if (i + 1 < components.size() && components[i + 1].type == TO) {
            if (i + 2 >= components.size()) {
                throw logic_error("to Use '-', you have to have two chars around it.");
            }
            NFABuilders.push(applyToOperation(components[i], components[i + 2]));
            i += 2;
        } else if (comp.type == POS_CLOSURE || comp.type == KLEENE_CLOSURE) {
            NFABuilders.push(applyClosure(comp, poll(NFABuilders)));
        } else if (comp.type == OPEN_BRACKETS) {
            NFABuilders.push(getBetweenBrackets(components, &i));
        } else if (comp.type == REG_EXP || comp.type == REG_DEF) {
            if (regToNFA.find(comp.regularDefinition) == regToNFA.end())
                throw logic_error(comp.regularDefinition + " Was not parsed.");
            NFABuilders.push(NFABuilder(regToNFA[comp.regularDefinition]));
        } else if (comp.type == CONCAT || comp.type == OR) {
            while (!operations.empty() && (operations.top() == CONCAT && comp.type == OR)) {
                NFABuilders.push(applyBinaryOperation(poll(operations), poll(NFABuilders), poll(NFABuilders)));
            }
            operations.push(comp.type);
        }
    }

    while (!operations.empty()) {
        NFABuilders.push(applyBinaryOperation(poll(operations), poll(NFABuilders), poll(NFABuilders)));
    }

    return NFABuilders.top().build();
}


NFABuilder NFACombiner::getBetweenBrackets(const vector<component> &components, int *index) {
    vector<component> temp;
    int bracketsCount = 1;
    int j;
    for (j = *index + 1; j < components.size(); j++) {
        if (components[j].type == OPEN_BRACKETS)
            bracketsCount++;
        if (components[j].type == CLOSE_BRACKETS)
            bracketsCount--;
        if (bracketsCount == 0) {
            break;
        } else if (bracketsCount < 0) {
            throw logic_error("Brackets are not balanced.");
        }
        temp.push_back(components[j]);
    }
    if (bracketsCount != 0) {
        throw logic_error("Brackets are not balanced.");
    }
    *index = j;
    return NFABuilder(regDefToNFA(temp));
}

NFABuilder NFACombiner::applyClosure(const component &comp, NFABuilder nfaBuilder) {
    return move(comp.type == POS_CLOSURE ? nfaBuilder.PositiveClosure() : nfaBuilder.KleeneClosure());
}

NFABuilder NFACombiner::applyBinaryOperation(const componentType type, NFABuilder f, NFABuilder s) {
    switch (type) {
        case CONCAT:
            return move(f.Concatenate(s.build()));
        case OR:
            return move(f.Or(s.build()));
        default:
            throw logic_error("Unknown Operation found.");
    }
}

NFABuilder NFACombiner::applyToOperation(const component &c1, const component &c2) {
    if (c1.regularDefinition.size() != 1 && c2.regularDefinition.size() != 1) {
        throw logic_error("Check '-' syntax, Just use one char at each end\"eg: a-z\".");
    }
    char first = c1.regularDefinition[0];
    char second = c2.regularDefinition[0];
    if (second - first < 0) throw logic_error("Check '-' syntax, eg: You can use a-z not z-a.");
    NFABuilder nfa_builder((NFA(first)));
    while (++first <= second) {
        nfa_builder.Or(NFA(first));
    }
    return move(nfa_builder);
}
