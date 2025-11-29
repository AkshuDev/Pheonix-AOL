#pragma once
#include <iostream>
#include <string>
#include <vector>

#include <colors.hpp>

enum class TokenType {
    TK_EOF,
    Unknown,
    Identifier,
    IntegerLiteral,
    FloatLiteral,
    DoubleLiteral,
    StringLiteral,
    CharLiteral,
    BoolLiteral,
    NullLiteral,

    // Keywords
    Function,
    VarDecl,
    ConstDecl,
    Let,
    Return,
    If,
    Else,
    While,
    For,
    Break,
    Continue,
    External,
    Unsafe,
    Assembly,
    True,
    False,
    Null,
    Switch,
    Case,
    Default,
    Enum,
    Struct,
    Class,
    Interface,
    Import,
    Module,

    // Types
    Var,
    Int,
    Uint,
    Float,
    Double,
    Char,
    Bool,
    String,
    Void,
    Auto,

    // Operators
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Increment,
    Decrement,
    Assign,
    PlusAssign,
    MinusAssign,
    StarAssign,
    SlashAssign,
    PercentAssign,
    Equal,
    NEqual,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,
    AndAnd,
    OrOr,
    Bang,
    BangEqual,
    EqualEqual,
    Amp,
    AmpAssign,
    Pipe,
    PipeAssign,
    Caret,
    CaretAssign,
    Tilde,
    ShiftLeft,
    ShiftLeftAssign,
    ShiftRight,
    ShiftRightAssign,
    Arrow,
    Question,
    Dot,

    // Delimiters
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Semicolon,
    Comma,
    Colon,
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int col;
};

class AOL_Lexer {
    public:
        AOL_Lexer(const std::string& source);

        Token nextToken();
        std::vector<Token> tokenize();
    
    private:
        char peek(int offset = 0) const;
        char advance();
        bool match(char expected);

        void skipWhitespace();
        void skipComment();

        Token identifierOrKeyword();
        Token number();
        Token stringLiteral();
        Token charLiteral();
        Token operatorOrDelimiter();

    private:
        std::string src;
        size_t pos = 0;
        int line = 1;
        int col = 1;
};

static inline void PrintToken(const Token& t) {
    using namespace Color;
    std::cout << Cyan << "[" << t.line << ":" << t.col << "] " << Reset;

    std::cout << Green << t.text << Reset << " -> " << Bold << Yellow << (int)t.type << Reset << "\n";
}
