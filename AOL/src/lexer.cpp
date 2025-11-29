#include <lexer.hpp>

#include <cctype>
#include <unordered_map>

static inline std::unordered_map<std::string, TokenType> KeywordMap = {
    {"fn", TokenType::Function},
    {"func", TokenType::Function}, // alias
    {"let", TokenType::VarDecl},
    {"const", TokenType::ConstDecl},
    {"var", TokenType::VarDecl},
    {"return", TokenType::Return},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"while", TokenType::While},
    {"for", TokenType::For},
    {"break", TokenType::Break},
    {"continue", TokenType::Continue},
    {"extern", TokenType::External},
    {"unsafe", TokenType::Unsafe},
    {"asm", TokenType::Assembly},
    {"true", TokenType::True},
    {"false", TokenType::False},
    {"null", TokenType::Null},
    {"switch", TokenType::Switch},
    {"case", TokenType::Case},
    {"default", TokenType::Default},
    {"enum", TokenType::Enum},
    {"struct", TokenType::Struct},
    {"class", TokenType::Class},
    {"interface", TokenType::Interface},
    {"import", TokenType::Import},
    {"module", TokenType::Module},
};

AOL_Lexer::AOL_Lexer(const std::string& source) : src(source) {}

char AOL_Lexer::peek(int offset) const {
    if (pos + offset >= src.size()) return '\0';
    return src[pos + offset];
}

char AOL_Lexer::advance() {
    char c = src[pos++];
    if (c == '\n') { line++; col = 1; }
    else col++;
    return c;
}

bool AOL_Lexer::match(char expected) {
    if (peek() != expected) return false;
    advance();
    return true;
}

void AOL_Lexer::skipWhitespace() {
    while (isspace(peek())) advance();
}

void AOL_Lexer::skipComment() {
    if (peek() == '/' && peek(1) == '/') {
        while (peek() != '\n' && peek() != '\0') advance();
    } else if (peek() == '/' && peek(1) == '*') {
        advance(); advance();
        while (!(peek() == '*' && peek(1) == '/')) advance();
        advance(); advance();
    }
}

Token AOL_Lexer::identifierOrKeyword() {
    int startCol = col;
    std::string text;
    while (isalnum(peek()) || peek() == '_')
        text += advance();

    if (KeywordMap.count(text))
        return {KeywordMap[text], text, line, startCol};

    return {TokenType::Identifier, text, line, startCol};
}

Token AOL_Lexer::number() {
    int startCol = col;
    std::string text;
    while (isdigit(peek()))
        text += advance();

    return {TokenType::IntegerLiteral, text, line, startCol};
}

Token AOL_Lexer::stringLiteral() {
    int startCol = col;
    advance(); // skip "
    std::string text;
    while (peek() != '"' && peek() != '\0')
        text += advance();
    advance(); // closing "
    return {TokenType::StringLiteral, text, line, startCol};
}

Token AOL_Lexer::charLiteral() {
    int startCol = col;
    advance(); // '
    char c = advance();
    advance(); // closing '
    return {TokenType::CharLiteral, std::string(1, c), line, startCol};
}

Token AOL_Lexer::operatorOrDelimiter() {
    int startCol = col;
    char c = advance();

    // Multi-char operators first
    if (c == '-' && match('>')) return {TokenType::Arrow, "->", line, startCol};
    if (c == '=' && match('=')) return {TokenType::EqualEqual, "==", line, startCol};
    if (c == '!' && match('=')) return {TokenType::NEqual, "!=", line, startCol};
    if (c == '<' && match('=')) return {TokenType::LessEqual, "<=", line, startCol};
    if (c == '>' && match('=')) return {TokenType::GreaterEqual, ">=", line, startCol};

    // Single char fallback
    switch(c) {
        case '+': return {TokenType::Plus, "+", line, startCol};
        case '-': return {TokenType::Minus, "-", line, startCol};
        case '*': return {TokenType::Star, "*", line, startCol};
        case '/': return {TokenType::Slash, "/", line, startCol};
        case '(': return {TokenType::LParen, "(", line, startCol};
        case ')': return {TokenType::RParen, ")", line, startCol};
        case '{': return {TokenType::LBrace, "{", line, startCol};
        case '}': return {TokenType::RBrace, "}", line, startCol};
        case ';': return {TokenType::Semicolon, ";", line, startCol};
        case ',': return {TokenType::Comma, ",", line, startCol};
        case '!': return {TokenType::Bang, "!", line, startCol};
    }
    return {TokenType::Unknown, std::string(1,c), line, startCol};
}

Token AOL_Lexer::nextToken() {
    skipWhitespace();
    skipComment();

    if (peek() == '\0') return {TokenType::TK_EOF, "", line, col};

    char c = peek();

    if (isalpha(c) || c == '_') return identifierOrKeyword();
    if (isdigit(c)) return number();
    if (c == '"') return stringLiteral();
    if (c == '\'') return charLiteral();

    return operatorOrDelimiter();
}

std::vector<Token> AOL_Lexer::tokenize() {
    std::vector<Token> result;
    Token t;
    while ((t = nextToken()).type != TokenType::TK_EOF)
        result.push_back(t);

    result.push_back(t);
    return result;
}
