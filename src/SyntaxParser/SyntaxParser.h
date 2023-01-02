#ifndef LEXICAL_ANALYZER_SYNTAXPARSER_H
#define LEXICAL_ANALYZER_SYNTAXPARSER_H

#include <string>
#include <vector>
#include <memory>
#include "GrammerTypes.h"
#include "ParsingTable.h"
#include "../Parser/LexicalParser.h"

class SyntaxParser {
public:
    SyntaxParser(const unordered_map<Symbol, Rule> &rules, const Symbol &staring_symbol);

    enum class Status {
        ACCEPTED,
        ACCEPTED_WITH_ERRORS,
        NOT_MATCHED
    };

    pair<vector<vector<Symbol>>, Status> parse(LexicalParser &tokenizer) const;

    bool fail() const;

private:
    bool has_error{};
    Symbol starting_symbol;
    unique_ptr<ParsingTable> table;

    enum class Behavior {
        MATCH_TERMINAL, ENTRY_EXISTS, SYNC_ENTRY, NO_ENTRY
    };

    SyntaxParser::Behavior get_behavior(const Symbol &cur_sym, const Symbol &token_sym) const;
};


#endif
