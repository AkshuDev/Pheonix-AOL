#include <parser.hpp>
#include <lexer.hpp>
#include <colors.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <memory>

AOL_Parser::AOL_Parser(const std::vector<Token>& toks) : tokens(toks) {}

Token AOL_Parser::peek(int offset) const {
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
        std::cerr << Color::Red << "Parse Error at " << peek().line << ":" << peek().col
                  << ": " << errMsg << "\n";
    }
}

static int precedence(TokenType t) {
    switch (t) {
        case TokenType::Star:
        case TokenType::Slash: return 3;
        case TokenType::Plus:
        case TokenType::Minus: return 2;
        case TokenType::EqualEqual:
        case TokenType::BangEqual:
        case TokenType::Less:
        case TokenType::LessEqual:
        case TokenType::Greater:
        case TokenType::GreaterEqual: return 1;
        default: return 0;
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
    for (;;) {
        Token op = peek();
        int p = precedence(op.type);
        if (p < minPrecedence || p == 0) break;
        advance();
        int nextMin = p;
        auto right = parseBinaryOp(nextMin);
        auto node = std::make_shared<ASTNode>(ASTNodeType::BinaryExpr, op.line, op.col);
        node->name = op.text;
        node->children.push_back(left);
        node->children.push_back(right);
        left = node;
    }
    return left;
}

std::shared_ptr<ASTNode> AOL_Parser::parseUnary() {
    Token t = peek();
    if (t.type == TokenType::Plus || t.type == TokenType::Minus || t.type == TokenType::Bang) {
        advance();
        auto right = parseUnary();
        auto node = std::make_shared<ASTNode>(ASTNodeType::UnaryExpr, t.line, t.col);
        node->name = t.text;
        node->children.push_back(right);
        return node;
    }
    return parsePrimary();
}

std::shared_ptr<ASTNode> AOL_Parser::parsePrimary() {
    Token t = peek();
    if (t.type == TokenType::Identifier) {
        advance();
        auto id = std::make_shared<ASTNode>(ASTNodeType::Identifier, t.line, t.col);
        id->name = t.text;
        return parseCallExpr(id);
    }
    if (t.type == TokenType::IntegerLiteral || t.type == TokenType::StringLiteral) {
        return parseLiteral();
    }
    if (t.type == TokenType::LParen) {
        advance();
        auto expr = parseExpression();
        expect(TokenType::RParen, "Expected ')'");
        return expr;
    }
    if (t.type == TokenType::Semicolon) {
    }
    advance();
    auto node = std::make_shared<ASTNode>(ASTNodeType::Literal, t.line, t.col);
    node->value = t.text;
    return node;

    switch (t.type) {
        case TokenType::Identifier: {
            advance();
            return parseCallExpr(std::make_shared<ASTNode>(ASTNodeType::Identifier, t.line, t.col, t.text));
        }
        case TokenType::IntegerLiteral:
        case TokenType::StringLiteral: {
            Token lit = advance();
            auto node = std::make_shared<ASTNode>(ASTNodeType::Literal, t.line, t.col);
            node->value = lit.text;
            return node;
        }
        case TokenType::LParen: {
            advance();
            auto expr = parseExpression();
            expect(TokenType::RParen, "Expected ')'");
            return expr;
        }
        default: {
            std::cerr << "Unexpected token in expression!\n";
            advance();
            return std::make_shared<ASTNode>(ASTNodeType::Error, t.line, t.col);
        }
    }
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
    if (!match(TokenType::LParen)) return callee;
    auto call = std::make_shared<ASTNode>(ASTNodeType::CallExpr, callee->line, callee->col);
    call->name = callee->name;
    if (!match(TokenType::RParen)) {
        for (;;) {
            call->children.push_back(parseExpression());
            if (match(TokenType::RParen)) break;
            expect(TokenType::Comma, "Expected ','");
        }
    }
    expect(TokenType::Semicolon, "Expected ';");
    return call;
}

std::shared_ptr<ASTNode> AOL_Parser::parseFunction() {
    expect(TokenType::Function, "Expected 'fn'");
    Token nameToken = advance();
    if (nameToken.type != TokenType::Identifier) {
        std::cerr << Color::Red << "Expected function name at " 
                  << nameToken.line << ":" << nameToken.col << "\n";
        return std::make_shared<ASTNode>(ASTNodeType::FunctionDecl, nameToken.line, nameToken.col);
    }

    auto node = std::make_shared<ASTNode>(ASTNodeType::FunctionDecl, nameToken.line, nameToken.col);
    node->name = nameToken.text;

    expect(TokenType::LParen, "Expected '(' after function name");

    while (peek().type != TokenType::RParen && !isAtEnd()) {
        Token paramToken = advance();
        if (paramToken.type != TokenType::Identifier) {
            std::cerr << Color::Red << "Expected parameter name at " << paramToken.line << ":" << paramToken.col << "\n";
            break;
        }

        auto paramNode = std::make_shared<ASTNode>(ASTNodeType::Identifier, paramToken.line, paramToken.col);
        paramNode->name = paramToken.text;
        node->params.push_back(paramNode);

        if (!match(TokenType::Comma)) break;
    }

    expect(TokenType::RParen, "Expected ')' after parameters");

    if (!match(TokenType::LBrace)) {
        std::cerr << Color::Red << "Expected '{' to start function body at " 
                  << peek().line << ":" << peek().col << "\n";
        return node;
    }

    while (!match(TokenType::RBrace) && !isAtEnd()) {
        node->children.push_back(parseStatement());
    }

    return node;
}

std::shared_ptr<ASTNode> AOL_Parser::parseVariableDecl() {
    Token declToken = advance(); // var, let, or const
    auto node = std::make_shared<ASTNode>(ASTNodeType::VariableDecl, declToken.line, declToken.col);

    Token nameToken = advance();
    if (nameToken.type != TokenType::Identifier) {
        std::cerr << Color::Red << "Expected variable name at " << nameToken.line << ":" << nameToken.col << "\n";
        return node;
    }
    node->name = nameToken.text; 

    if (match(TokenType::Equal)) {
        node->children.push_back(parseExpression());
    }

    expect(TokenType::Semicolon, "Expected ';' after variable declaration!");
    return node;
}

std::shared_ptr<ASTNode> AOL_Parser::parseReturn() {
    Token retToken = advance(); // 'ret'
    auto node = std::make_shared<ASTNode>(ASTNodeType::ReturnStmt, retToken.line, retToken.col);

    if (peek().type != TokenType::Semicolon) {
        node->children.push_back(parseExpression());
    }

    expect(TokenType::Semicolon, "Expected ';'");
    return node;
}

std::shared_ptr<ASTNode> AOL_Parser::parseIf() {
    Token ifToken = advance(); // 'if'
    auto node = std::make_shared<ASTNode>(ASTNodeType::IfStmt, ifToken.line, ifToken.col);

    expect(TokenType::LParen, "Expected '(' after 'if'");
    node->children.push_back(parseExpression()); // condition
    expect(TokenType::RParen, "Expected ')' after condition");

    if (match(TokenType::LBrace)) {
        while (!match(TokenType::RBrace) && !isAtEnd()) {
            node->children.push_back(parseStatement());
        }
    } else {
        node->children.push_back(parseStatement());
    }

    if (match(TokenType::Else)) {
        if (match(TokenType::LBrace)) {
            auto elseNode = std::make_shared<ASTNode>(ASTNodeType::StmtBlock, peek().line, peek().col);
            while (!match(TokenType::RBrace) && !isAtEnd()) {
                elseNode->children.push_back(parseStatement());
            }
            node->children.push_back(elseNode);
        } else {
            node->children.push_back(parseStatement());
        }
    }

    return node;
}

std::shared_ptr<ASTNode> AOL_Parser::parseWhile() {
    Token whileToken = advance(); // 'while'
    auto node = std::make_shared<ASTNode>(ASTNodeType::WhileStmt, whileToken.line, whileToken.col);

    expect(TokenType::LParen, "Expected '(' after 'while'");
    node->children.push_back(parseExpression()); // condition
    expect(TokenType::RParen, "Expected ')' after condition");

    if (match(TokenType::LBrace)) {
        while (!match(TokenType::RBrace) && !isAtEnd()) {
            node->children.push_back(parseStatement());
        }
    } else {
        node->children.push_back(parseStatement());
    }

    return node;
}

std::shared_ptr<ASTNode> AOL_Parser::parseFor() {
    Token forToken = advance(); // 'for'
    auto node = std::make_shared<ASTNode>(ASTNodeType::ForStmt, forToken.line, forToken.col);

    expect(TokenType::LParen, "Expected '(' after 'for'");

    if (peek().type != TokenType::Semicolon) {
        if (peek().type == TokenType::VarDecl || peek().type == TokenType::ConstDecl || peek().type == TokenType::Let) {
            node->children.push_back(parseVariableDecl());
        } else {
            node->children.push_back(parseExpression());
        }
    }
    expect(TokenType::Semicolon, "Expected, ';'");

    if (peek().type != TokenType::Semicolon)
        node->children.push_back(parseExpression());
    expect(TokenType::Semicolon, "Expected ';'");

    if (peek().type != TokenType::RParen)
        node->children.push_back(parseExpression());
    expect(TokenType::RParen, "Expected ')'");

    if (match(TokenType::LBrace)) {
        while (!match(TokenType::RBrace) && !isAtEnd()) {
            node->children.push_back(parseStatement());
        }
    } else {
        node->children.push_back(parseStatement());
    }

    return node;
}

std::shared_ptr<ASTNode> AOL_Parser::parseBreak() {
    Token token = advance(); // 'break'
    auto node = std::make_shared<ASTNode>(ASTNodeType::BreakStmt, token.line, token.col);
    match(TokenType::Semicolon);
    return node;
}

std::shared_ptr<ASTNode> AOL_Parser::parseContinue() {
    Token token = advance(); // 'continue'
    auto node = std::make_shared<ASTNode>(ASTNodeType::ContinueStmt, token.line, token.col);
    match(TokenType::Semicolon);
    return node;
}
