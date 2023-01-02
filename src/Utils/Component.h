#ifndef LEXICAL_ANALYZER_COMPONENT_H
#define LEXICAL_ANALYZER_COMPONENT_H

#include <utility>
#include "string"

using namespace std;

const char EPSILON = 0;


enum componentType {
    REG_EXP,
    REG_DEF,
    CLOSE_BRACKETS,
    OPEN_BRACKETS,
    POS_CLOSURE,
    KLEENE_CLOSURE,
    CONCAT,
    OR,
    TO,
    EQUAL
};

struct component {
    component(componentType type, string regDef) : type(type), regularDefinition(move(regDef)) {}

    explicit component(componentType type) : type(type), regularDefinition("") {}

    friend bool operator==(const component &lhs, const component &rhs) {
        return lhs.type == rhs.type && lhs.regularDefinition == rhs.regularDefinition;
    }

    componentType type;
    string regularDefinition;
};

#endif //LEXICAL_ANALYZER_COMPONENT_H
