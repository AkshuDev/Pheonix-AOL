#pragma once

#include <string>
#include <vector>
#include <memory>

#include <lexer.hpp>

enum class ASTNodeType {
    Program,
    FunctionDecl,
    VariableDecl,
    ConstDecl,
    ReturnStmt,
    IfStmt,
    WhileStmt,
    ForStmt,
    BreakStmt,
    ContinueStmt,
    Expression,
    BinaryExpr,
    UnaryExpr,
    Literal,
    Identifier,
    CallExpr,
};

struct ASTNode;

struct ASTNode {
    ASTNodeType type;
    std::string name; // variable, function name, or operator
    std::string value; // literal value
    std::vector<std::shared_ptr<ASTNode>> children;
    std::vector<std::shared_ptr<ASTNode>> params; // Only for functions!
    int line = 0;
    int col = 0;

    ASTNode(ASTNodeType t, int l=0, int c=0) : type(t), line(l), col(c) {}
};

class AOL_Parser {
public:
    AOL_Parser(const std::vector<Token>& tokens);
    
    std::shared_ptr<ASTNode> parseProgram();
    
private:
    const std::vector<Token>& tokens;
    size_t pos = 0;

    Token peek(int offset = 0) const;
    Token advance();
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    void expect(TokenType type, const std::string& errMsg);

    std::shared_ptr<ASTNode> parseFunction();
    std::shared_ptr<ASTNode> parseStatement();
    std::shared_ptr<ASTNode> parseVariableDecl();
    std::shared_ptr<ASTNode> parseReturn();
    std::shared_ptr<ASTNode> parseIf();
    std::shared_ptr<ASTNode> parseWhile();
    std::shared_ptr<ASTNode> parseFor();
    std::shared_ptr<ASTNode> parseBreak();
    std::shared_ptr<ASTNode> parseContinue();

    std::shared_ptr<ASTNode> parseExpression();
    std::shared_ptr<ASTNode> parseBinaryOp(int minPrecedence = 0);
    std::shared_ptr<ASTNode> parseUnary();
    std::shared_ptr<ASTNode> parsePrimary();
    std::shared_ptr<ASTNode> parseLiteral();
    std::shared_ptr<ASTNode> parseIdentifier();
    std::shared_ptr<ASTNode> parseCallExpr(std::shared_ptr<ASTNode> callee);

    bool isAtEnd() const { return pos >= tokens.size() || tokens[pos].type == TokenType::TK_EOF; }
};
