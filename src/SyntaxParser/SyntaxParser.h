#ifndef LEXICAL_ANALYZER_SYNTAXPARSER_H
#define LEXICAL_ANALYZER_SYNTAXPARSER_H

#include <string>
#include <vector>
#include <memory>
#include "GrammarTypes.h"
#include "ParsingTable.h"
#include "../Parser/LexicalParser.h"

class SyntaxParser {
public:
    SyntaxParser(const unordered_map<Symbol, Rule> &rules, const Symbol &staringSymbol);

    enum class Status {
        ACCEPTED,
        ACCEPTED_WITH_ERRORS,
        NOT_MATCHED
    };

    pair<vector<vector<Symbol>>, Status> parse(LexicalParser &tokenizer) const;

    bool fail() const;

private:
    bool hasError{};
    Symbol startingSymbol;
    unique_ptr<ParsingTable> table;

    enum class Behavior {
        MATCH_TERMINAL,
        ENTRY_EXISTS,
        SYNC_ENTRY,
        NO_ENTRY
    };

    SyntaxParser::Behavior getBehavior(const Symbol &topOfStack, const Symbol &tokenSym) const;
};


#endif
