#include <parser.hpp>
#include <lexer.hpp>
#include <colors.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <memory>

AOL_Parser::AOL_Parser(const std::vector<Token>& toks) : tokens(toks) {}

Token AOL_Parser::peek(int offset = 0) const {
    if (pos + offset >= tokens.size()) return Token{TokenType::TK_EOF, "", 0, 0};
    return tokens[pos + offset];
}

Token AOL_Parser::advance() {
    if (!isAtEnd()) return tokens[pos++];
    return Token{TokenType::TK_EOF, "", 0, 0};
}

bool AOL_Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

bool AOL_Parser::match(const std::vector<TokenType>& types) {
    for (auto t : types) {
        if (peek().type == t) {
            advance();
            return true;
        }
    }
    return false;
}

void AOL_Parser::expect(TokenType type, const std::string& errMsg) {
    if (!match(type)) {
        std::cerr << Color::Red << "Parse Error at " << peek().line << ":" << peek().col << ": " << errMsg << "\n";
    }
}

std::shared_ptr<ASTNode> AOL_Parser::parseProgram() {
    auto program = std::make_shared<ASTNode>(ASTNodeType::Program);
    while (!isAtEnd()) {
        program->children.push_back(parseStatement());
    }
    return program;
}

std::shared_ptr<ASTNode> AOL_Parser::parseStatement() {
    Token t = peek();
    switch (t.type) {
        case TokenType::Function:   return parseFunction();
        case TokenType::VarDecl:
        case TokenType::Let:
        case TokenType::ConstDecl:  return parseVariableDecl();
        case TokenType::Return:     return parseReturn();
        case TokenType::If:         return parseIf();
        case TokenType::While:      return parseWhile();
        case TokenType::For:        return parseFor();
        case TokenType::Break:      return parseBreak();
        case TokenType::Continue:   return parseContinue();
        default:                    return parseExpression();
    }
}

std::shared_ptr<ASTNode> AOL_Parser::parseExpression() {
    return parseBinaryOp(0);
}

std::shared_ptr<ASTNode> AOL_Parser::parseBinaryOp(int minPrecedence) {
    auto left = parseUnary();
    // TODO: implement operator precedence parsing
    return left;
}

std::shared_ptr<ASTNode> AOL_Parser::parseUnary() {
    // TODO: handle unary +,-,! operators
    return parsePrimary();
}

std::shared_ptr<ASTNode> AOL_Parser::parsePrimary() {
    Token t = advance();
    if (t.type == TokenType::Identifier) {
        return parseCallExpr(std::make_shared<ASTNode>(ASTNodeType::Identifier, t.line, t.col));
    }
    return parseLiteral();
}

std::shared_ptr<ASTNode> AOL_Parser::parseLiteral() {
    Token t = advance();
    auto node = std::make_shared<ASTNode>(ASTNodeType::Literal, t.line, t.col);
    node->value = t.text;
    return node;
}

std::shared_ptr<ASTNode> AOL_Parser::parseIdentifier() {
    Token t = advance();
    auto node = std::make_shared<ASTNode>(ASTNodeType::Identifier, t.line, t.col);
    node->name = t.text;
    return node;
}

std::shared_ptr<ASTNode> AOL_Parser::parseCallExpr(std::shared_ptr<ASTNode> callee) {
    // TODO: parse function arguments
    return callee;
}