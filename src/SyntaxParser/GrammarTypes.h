#include <utility>

#ifndef LEXICAL_ANALYZER_GRAMMARTYPES_H
#define LEXICAL_ANALYZER_GRAMMARTYPES_H

using namespace std;

struct Symbol {
    enum class Type {
        TERMINAL,
        NON_TERMINAL,
        EPSILON
    };
    string name;
    Type type;

    inline bool operator==(const Symbol &a) const {
        return name == a.name && type == a.type;
    }

    inline bool operator<(const Symbol &a) const {
        if (type != a.type)
            return type < a.type;
        return name < a.name;
    }
};

namespace std {
    template<>
    struct hash<Symbol> {
        size_t
        operator()(const Symbol &symbol) const noexcept {
            return hash<string>()(symbol.name);
        }
    };
}

const Symbol epsSymbol = {"\\L", Symbol::Type::EPSILON};

using Production = vector<Symbol>;

class Rule : public vector<Production> {
public:
    explicit Rule(string lhsName) {
        lhs = {move(lhsName), Symbol::Type::NON_TERMINAL};
    }

    explicit Rule(string lhsName, vector<Production> vec) : vector<Production>(move(vec)) {
        lhs = {move(lhsName), Symbol::Type::NON_TERMINAL};
    }

    const Symbol &getLhs() const {
        return this->lhs;
    }

private:
    Symbol lhs;
};

const Production SYNC_PRODUCTION = {};

#endif
