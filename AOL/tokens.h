#ifndef TOKENS_H
#define TOKENS_H
#include <iostream>
#include <string>

namespace AOL_TOKEN{
    enum class TokenType {
        ASSIGN,
        INT,
        CHAR,
        STRING,
        FLOAT,
        VAR,
        ENDLINE,
        IDENTIFIER,
        INT_VAR,
        CHAR_VAR,
        STRING_VAR,
        FLOAT_VAR,
        ADD,
        SUB,
        MULT,
        DIV,
        FDIV,
        EXPO,
        MOD,
        LPAR,
        RPAR,
        XOR,
        AND,
        OR,
        NOT,
        IF,
        ELSE_IF,
        ELSE,
        BOOL,
        BOOL_VAR,
        TRUE_,
        FALSE_,
        SWITCH,
        CASE,
        GREATER_THAN,
        LESS_THAN,
        EQUALS,
        GREATER_OR_EQUALS,
        LESS_OR_EQUALS,
        NOT_EQUALS,
        CODE_BLOCK_OPEN,
        CODE_BLOCK_CLOSE,
        UNKNOWN
    };

    struct Token {
        TokenType type;
        std::string value;
    };

}

#endif