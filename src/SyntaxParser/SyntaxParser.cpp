#include <memory>
#include <queue>
#include <algorithm>
#include "SyntaxParser.h"
#include "LL1GrammarBuilder.h"


SyntaxParser::SyntaxParser(const unordered_map<Symbol, Rule> &rules,
                           const Symbol &staringSymbol) : hasError(false) {
    SyntaxUtils utils{rules, staringSymbol};
    this->startingSymbol = staringSymbol;
    this->table = make_unique<ParsingTable>(rules, utils);
    hasError = table->fail();
}


SyntaxParser::Behavior SyntaxParser::getBehavior(const Symbol &topOfStack, const Symbol &tokenSym) const {
    if (topOfStack.type == Symbol::Type::TERMINAL) {
        return Behavior::MATCH_TERMINAL;
    }
    if (!table->hasProduction(topOfStack, tokenSym)) {
        return Behavior::NO_ENTRY;
    }
    if (table->getProduction(topOfStack, tokenSym) == SYNC_PRODUCTION) {
        return Behavior::SYNC_ENTRY;
    }
    return Behavior::ENTRY_EXISTS;
}

class LexicalParserWrapper {
public:
    explicit LexicalParserWrapper(LexicalParser &parser) : parser(parser) { }

    bool getToken(Token &token) {
        if (parser.getToken(token)) {
            return true;
        }
        if (state == State::RETURN_ENDING_SYMBOL) {
            token = {"$", "$"};
            return true;
        }
        return false;
    }

    void nextToken() {
        parser.nextToken();
        if (Token temp; !getToken(temp) && state == State::RETURN_FROM_PARSER) {
            state = State::RETURN_ENDING_SYMBOL;
            return;
        }
        if (state == State::RETURN_ENDING_SYMBOL) {
            state = State::EMPTY_BUFFER;
        }
    }

private:
    LexicalParser &parser;
    enum class State {
        RETURN_FROM_PARSER,
        RETURN_ENDING_SYMBOL,
        EMPTY_BUFFER
    };
    State state = State::RETURN_FROM_PARSER;
};

pair<vector<vector<Symbol>>, SyntaxParser::Status>
SyntaxParser::parse(LexicalParser &parser) const {
    LexicalParserWrapper tokenizer(parser);
    vector<Symbol> matchedTerminals;
    vector<vector<Symbol>> derivation;

    vector<Symbol> parsingStack;
    parsingStack.push_back(Symbol{"$", Symbol::Type::TERMINAL});
    parsingStack.push_back(this->startingSymbol);
    derivation.push_back({this->startingSymbol});

    Status status = Status::ACCEPTED;

    auto storeDerivation = [&]() {
        derivation.push_back(matchedTerminals);
        if (parsingStack.size() > 1) {
            derivation.back().insert(
                    derivation.back().end(),
                    parsingStack.rbegin(),
                    prev(parsingStack.rend()));
        }
    };

    Token curToken;
    while (!parsingStack.empty() && tokenizer.getToken(curToken)) {

        const Symbol tokenSym{curToken.regExp, Symbol::Type::TERMINAL};
        const Symbol &topOfStack = parsingStack.back();

        switch (getBehavior(topOfStack, tokenSym)) {
            case Behavior::MATCH_TERMINAL: {
                parsingStack.pop_back();
                if (topOfStack.name == tokenSym.name) {
                    matchedTerminals.push_back(topOfStack);
                    tokenizer.nextToken();
                } else {
                    cerr << "Error: missing " << topOfStack.name << ", inserted.\n";
                    matchedTerminals.push_back(topOfStack);
                    status = Status::ACCEPTED_WITH_ERRORS;
                }
                break;
            }
            case Behavior::ENTRY_EXISTS: {
                const Production &prod = table->getProduction(topOfStack, tokenSym);
                parsingStack.pop_back();
                for_each(prod.rbegin(), prod.rend(), [&](const Symbol &symbol) {
                    if (symbol == epsSymbol) return;
                    parsingStack.push_back(symbol);
                });
                storeDerivation();
                break;
            }
            case Behavior::SYNC_ENTRY: {
                parsingStack.pop_back();
                cerr << "Error, Table[" << topOfStack.name << ", " << tokenSym.name << "] = synch "
                     << topOfStack.name << " has been popped.\n";
                status = Status::ACCEPTED_WITH_ERRORS;
                storeDerivation();
                break;
            }
            case Behavior::NO_ENTRY: {
                tokenizer.nextToken();
                cerr << "Error: (illegal " << topOfStack.name << ") - discard " << tokenSym.name << " \""
                     << curToken.matchString << "\".\n";
                status = Status::ACCEPTED_WITH_ERRORS;
                break;
            }
        }
    }
    Token token;
    if (parsingStack.empty() != (!tokenizer.getToken(token))) {
        status = Status::NOT_MATCHED;
    }
    return {derivation, status};
}

bool SyntaxParser::fail() const {
    return this->hasError;
}
