#include <memory>
#include <queue>
#include <algorithm>
#include "SyntaxParser.h"
#include "LL1GrammarBuilder.h"


SyntaxParser::SyntaxParser(const unordered_map<Symbol, Rule> &rules, const Symbol &staring_symbol) : has_error(
        false) {
    SyntaxUtils utils{rules, staring_symbol};

    this->starting_symbol = staring_symbol;
    this->table = make_unique<ParsingTable>(rules, utils);

    has_error = table->fail();
}


SyntaxParser::Behavior SyntaxParser::get_behavior(const Symbol &cur_sym, const Symbol &token_sym) const {
    if (cur_sym.type == Symbol::Type::TERMINAL) {
        return Behavior::MATCH_TERMINAL;
    }
    if (!table->hasProduction(cur_sym, token_sym)) {
        return Behavior::NO_ENTRY;
    }
    if (table->getProduction(cur_sym, token_sym) == SYNC_PRODUCTION) {
        return Behavior::SYNC_ENTRY;
    }
    return Behavior::ENTRY_EXISTS;
}

class Lexical_parser_wrapper {
public:
    explicit Lexical_parser_wrapper(LexicalParser &parser) : parser(parser) {
    }

    bool get_token(Token &token) {
        if (parser.getToken(token)) {
            return true;
        }
        if (state == State::RET_ENDING_SYMBOL) {
            token = {"$", "$"};
            return true;
        }
        return false;
    }

    void next_token() {
        parser.nextToken();
        // Add $ at the end for syntax parser if buffer is empty
        if (Token temp; !get_token(temp) && state == State::RET_FROM_PARSER) {
            state = State::RET_ENDING_SYMBOL;
            return;
        }
        // Calling next_token after ending symbol.
        if (state == State::RET_ENDING_SYMBOL) {
            state = State::EMPTY_BUFFER;
        }
    }

private:
    LexicalParser &parser;
    enum class State {
        RET_FROM_PARSER, RET_ENDING_SYMBOL, EMPTY_BUFFER
    };
    State state = State::RET_FROM_PARSER;
};

pair<vector<vector<Symbol>>, SyntaxParser::Status>
SyntaxParser::parse(LexicalParser &parser) const {
    Lexical_parser_wrapper tokenizer(parser);
    vector<Symbol> matched_terminals;
    vector<vector<Symbol>> derivation;

    vector<Symbol> stk;
    stk.push_back(Symbol{"$", Symbol::Type::TERMINAL});
    stk.push_back(this->starting_symbol);
    derivation.push_back({this->starting_symbol});

    Status status = Status::ACCEPTED;

    auto store_derivation = [&]() {
        derivation.push_back(matched_terminals);
        if (stk.size() > 1) {
            derivation.back().insert(derivation.back().end(), stk.rbegin(), prev(stk.rend()));
        }
    };

    Token curToken;
    while (!stk.empty() && tokenizer.get_token(curToken)) {

        const Symbol token_sym{curToken.regExp, Symbol::Type::TERMINAL};
        const Symbol &cur_sym = stk.back();

        switch (get_behavior(cur_sym, token_sym)) {
            // Matches and pops two terminal symbols if they are equal
            // Error recovery: If they are not equal, the parser pops that
            // unmatched terminal symbol from the stack and it issues an error
            // message saying that that unmatched terminal is inserted.
            case Behavior::MATCH_TERMINAL: {
                stk.pop_back();
                if (cur_sym.name == token_sym.name) {
                    matched_terminals.push_back(cur_sym);
                    tokenizer.next_token();
                } else {
                    cerr << "Error: missing " << cur_sym.name << ", inserted.\n";
                    matched_terminals.push_back(cur_sym);
                    status = Status::ACCEPTED_WITH_ERRORS;
                }
                break;
            }
                // Pops non-terminal from the stack and pushes the matched production
                // in reverse order to the stack.
            case Behavior::ENTRY_EXISTS: {
                const Production &prod = table->getProduction(cur_sym, token_sym);
                stk.pop_back();
                for_each(prod.rbegin(), prod.rend(), [&](const Symbol &symbol) {
                    if (symbol == epsSymbol) return;
                    stk.push_back(symbol);
                });
                store_derivation();
                break;
            }
                // Error recovery: The parser will pop the non-terminal from the stack and
                // continues from that state.
            case Behavior::SYNC_ENTRY: {
                stk.pop_back();
                cerr << "Error, Table[" << cur_sym.name << ", " << token_sym.name << "] = synch "
                     << cur_sym.name << " has been popped.\n";
                status = Status::ACCEPTED_WITH_ERRORS;
                store_derivation();
                break;
            }
                // Error recovery: For an empty entry, the input symbol is discarded.
            case Behavior::NO_ENTRY: {
                tokenizer.next_token();
                cerr << "Error: (illegal " << cur_sym.name << ") - discard " << token_sym.name << " \""
                     << curToken.matchString << "\".\n";
                status = Status::ACCEPTED_WITH_ERRORS;
                break;
            }
        }
    }
    Token token;
    if (stk.empty() != (!tokenizer.get_token(token))) {
        status = Status::NOT_MATCHED;
    }
    return {derivation, status};
}

bool SyntaxParser::fail() const {
    return this->has_error;
}
