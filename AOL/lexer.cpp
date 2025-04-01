#include <iostream>
#include <string>
#include <cstring>
#include <cctype>
#include <algorithm>

#include "lexer.h"

Lexer_A::Lexer_A(const std::string& source) : sourceCode(source), pos(0) {}

char Lexer_A::peek() const {
    return pos < sourceCode.length() ? sourceCode[pos] : '\0';  // fixed typo
}

// Helper function to peek the next character without advancing
char Lexer_A::peekNext() {
    if (pos + 1 < sourceCode.length()) {
        return sourceCode[pos + 1];  // Return the next character
    }
    return '\0';  // Return a null character if we've reached the end
}

char Lexer_A::advance() {
    return pos < sourceCode.length() ? sourceCode[pos++] : '\0';
}

AOL_TOKEN::Token Lexer_A::makeToken(AOL_TOKEN::TokenType type, const std::string& value) {
    return {type, value};
}

AOL_TOKEN::Token Lexer_A::parseIdentifier() {
    std::string value;
    while (std::isalnum(peek())) {
        value += advance();
    }

    if (value == "if") return makeToken(AOL_TOKEN::TokenType::IF, value);
    if (value == "else"){
        if (peek() == ' ') {
            while (std::isalnum(peek())) {
                value += advance();
            }
        }

        if (value == "else if") return makeToken(AOL_TOKEN::TokenType::ELSE_IF, value);
        if (value == "else") return makeToken(AOL_TOKEN::TokenType::ELSE, value);
    }

    if (value == "int") return makeToken(AOL_TOKEN::TokenType::INT_VAR, value);
    if (value == "char") return makeToken(AOL_TOKEN::TokenType::CHAR_VAR, value);
    if (value == "string") return makeToken(AOL_TOKEN::TokenType::STRING_VAR, value);
    if (value == "float") return makeToken(AOL_TOKEN::TokenType::FLOAT_VAR, value);
    if (value == "var") return makeToken(AOL_TOKEN::TokenType::VAR, value);
    if (value == "and") return makeToken(AOL_TOKEN::TokenType::AND, value);
    if (value == "not") return makeToken(AOL_TOKEN::TokenType::NOT, value);
    if (value == "or") return makeToken(AOL_TOKEN::TokenType::OR, value);
    if (value == "bool") return makeToken(AOL_TOKEN::TokenType::BOOL_VAR, value);
    if (value == "true") return makeToken(AOL_TOKEN::TokenType::TRUE_, value);
    if (value == "false") return makeToken(AOL_TOKEN::TokenType::FALSE_, value);

    return makeToken(AOL_TOKEN::TokenType::IDENTIFIER, value);
}

AOL_TOKEN::Token Lexer_A::parseNumber() {
    std::string value;

    while (std::isdigit(peek())) {
        value += advance();
    }

    if (peek() == '.') {  // Floating point
        value += advance();
        while (std::isdigit(peek())) {
            value += advance();
        }
        return makeToken(AOL_TOKEN::TokenType::FLOAT, value);
    }

    return makeToken(AOL_TOKEN::TokenType::INT, value);
}

AOL_TOKEN::Token Lexer_A::parseString() {
    std::string value;
    advance(); // Consume opening "
    while (peek() != '"' && peek() != '\0') {
        value += advance();
    }
    if (peek() == '"') advance(); // Consume closing "
    return makeToken(AOL_TOKEN::TokenType::STRING, value);
}

AOL_TOKEN::Token Lexer_A::parseChar() {
    std::string value;
    advance(); // Consume opening '
    if (peek() != '\0' && peekNext() == '\'' && pos + 2 < sourceCode.length()) {
        value += advance(); // Handle escape characters
        value += advance();
    } else {
        value += advance();
    }
    if (peek() == '\'') advance(); // Consume closing '
    return makeToken(AOL_TOKEN::TokenType::CHAR, value);
}

std::vector<AOL_TOKEN::Token> Lexer_A::tokenize() {
    std::vector<AOL_TOKEN::Token> tokens;

    while (pos < sourceCode.length()) {
        char c = peek();

        if (std::isspace(c)) {
            if (c == '\n') {  // Explicitly handle newlines
                if (tokens.empty() || tokens.back().type != AOL_TOKEN::TokenType::ENDLINE) {
                    tokens.push_back(makeToken(AOL_TOKEN::TokenType::ENDLINE, "\\n"));
                } else {
                    advance();
                    continue;
                }
            }
            advance();
            continue;
        }

        if (c == '=') {
            if (peekNext() == '=') {
                tokens.push_back(makeToken(AOL_TOKEN::TokenType::EQUALS, std::string(1, advance()) + advance()));
            } else {
                tokens.push_back(makeToken(AOL_TOKEN::TokenType::ASSIGN, std::string(1, advance())));
            }
            continue;
        }

        if (c == ';') {
            tokens.push_back(makeToken(AOL_TOKEN::TokenType::ENDLINE, std::string(1, advance())));
            continue;
        }

        if (c == '+') {
            tokens.push_back(makeToken(AOL_TOKEN::TokenType::ADD, std::string(1, advance())));
            continue;
        }

        if (c == '-') {
            tokens.push_back(makeToken(AOL_TOKEN::TokenType::SUB, std::string(1, advance())));
            continue;
        }

        if (c == '*') {
            if (peekNext() == '*') {
                tokens.push_back(makeToken(AOL_TOKEN::TokenType::EXPO, std::string(1, advance()) + advance()));
            } else {
                tokens.push_back(makeToken(AOL_TOKEN::TokenType::MULT, std::string(1, advance())));
            }
            continue;
        }

        if (c == '/') {
            if (peekNext() == '/') {
                tokens.push_back(makeToken(AOL_TOKEN::TokenType::FDIV, std::string(1, advance()) + advance()));
            } else {
                tokens.push_back(makeToken(AOL_TOKEN::TokenType::DIV, std::string(1, advance())));
            }
            continue;
        }

        if (c == '%') {
            tokens.push_back(makeToken(AOL_TOKEN::TokenType::MOD, std::string(1, advance())));
            continue;
        }

        if (c == '^') {
            tokens.push_back(makeToken(AOL_TOKEN::TokenType::XOR, std::string(1, advance())));
            continue;
        }

        if (c == '(') {
            tokens.push_back(makeToken(AOL_TOKEN::TokenType::LPAR, std::string(1, advance())));
            continue;
        }

        if (c == ')') {
            tokens.push_back(makeToken(AOL_TOKEN::TokenType::RPAR, std::string(1, advance())));
            continue;
        }

        if (c == '&') {
            if (peekNext() == '&') {
                tokens.push_back(makeToken(AOL_TOKEN::TokenType::AND, std::string(1, advance()) + advance()));
            }
            continue;
        }

        if (c == '|') {
            if (peekNext() == '|') {
                tokens.push_back(makeToken(AOL_TOKEN::TokenType::OR, std::string(1, advance()) + advance()));
            }
            continue;
        }

        if (c == '!') {
            if (peekNext() == '=') {
                tokens.push_back(makeToken(AOL_TOKEN::TokenType::NOT_EQUALS, std::string(1, advance()) + advance()));
            } else {
                tokens.push_back(makeToken(AOL_TOKEN::TokenType::NOT, std::string(1, advance())));
            }
            continue;
        }

        if (c == '\"') {
            tokens.push_back(parseString());
        }

        if (c == '\'') {
            tokens.push_back(parseChar());
        }

        if (c == '>') {
            if (peekNext() == '=') {
                tokens.push_back(makeToken(AOL_TOKEN::TokenType::GREATER_OR_EQUALS, std::string(1, advance()) + advance()));
            } else {
                tokens.push_back(makeToken(AOL_TOKEN::TokenType::GREATER_THAN, std::string(1, advance())));
            }
            continue;
        }

        if (c == '<') {
            if (peekNext() == '=') {
                tokens.push_back(makeToken(AOL_TOKEN::TokenType::LESS_OR_EQUALS, std::string(1, advance()) + advance()));
            } else {
                tokens.push_back(makeToken(AOL_TOKEN::TokenType::LESS_THAN, std::string(1, advance())));
            }
            continue;
        }

        if (c == '}') {
            tokens.push_back(makeToken(AOL_TOKEN::TokenType::CODE_BLOCK_CLOSE, std::string(1, advance())));
        }

        if (c == '{') {
            tokens.push_back(makeToken(AOL_TOKEN::TokenType::CODE_BLOCK_OPEN, std::string(1, advance())));
        }

        if (std::isalpha(c)) {
            tokens.push_back(parseIdentifier());
            continue;
        }

        if(std::isdigit(c)) {
            tokens.push_back(parseNumber());
            continue;
        }

        tokens.push_back(makeToken(AOL_TOKEN::TokenType::UNKNOWN, std::string(1, advance())));
    }

    return tokens;
}
