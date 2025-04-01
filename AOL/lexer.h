#ifndef LEXER_H
#define LEXER_H

// Project includes
#include "tokens.h"
// Std includes
#include <vector>
#include <cctype>

class Lexer_A {
    public:
        explicit Lexer_A(const std::string& source);  // Constructor

        std::vector<AOL_TOKEN::Token> tokenize();

    private:
        std::string sourceCode;
        size_t pos;

        char peek() const;

        char peekNext();
        AOL_TOKEN::Token parseNumber();
        char advance();
        AOL_TOKEN::Token makeToken(AOL_TOKEN::TokenType type, const std::string& value);
        AOL_TOKEN::Token parseIdentifier();
        AOL_TOKEN::Token parseString();
        AOL_TOKEN::Token parseChar();
};

#endif