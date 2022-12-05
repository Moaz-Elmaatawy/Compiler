#include "Token.h"

using namespace std;

string Token::GetType() {
    return this->type;
}

string Token::GetLexeme() {
    return this->lexeme;
}

Token::Token(string typeIn, string lexemeIn) {
    this->type=typeIn;
    this->lexeme=lexemeIn;
}

Token::Token() = default;
