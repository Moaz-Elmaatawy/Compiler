#include <string>

using namespace std;

#ifndef TOKEN_H
#define TOKEN_H
class Token{
    private:
        string type;
        string lexeme;
    public:
        Token();
        Token(string type,string lexeme);
        string GetType();
        string GetLexeme();
};
#endif //TOKEN_H
