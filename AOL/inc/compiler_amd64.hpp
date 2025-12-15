#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <parser.hpp>
#include <sstream>
#include <iosfwd>

struct VariableInfo {
    std::string name;
    int offset; // relative to rbp
    int size; // in bytes
    std::string reg; // register
};

struct FunctionSymbol {
    std::string name;
    std::vector<VariableInfo> params;
    std::vector<VariableInfo> locals;
    int stackSize; // total stack size for locals
    std::shared_ptr<ASTNode> body;
};

class Compiler_Amd64 {
public:
    Compiler_Amd64();
    ~Compiler_Amd64() = default;

    std::string compile(const std::shared_ptr<ASTNode>& program);

private:
    std::string compileProgram(const std::shared_ptr<ASTNode>& node);
    std::string compileStatement(const std::shared_ptr<ASTNode>& node, const std::string& targetReg = "%rax");
    std::string compileFunction(const std::shared_ptr<ASTNode>& node);
    std::string compileVariableDecl(const std::shared_ptr<ASTNode>& node, const std::string& targetReg = "%rax");
    std::string compileReturn(const std::shared_ptr<ASTNode>& node, const std::string& targetReg = "%rax");
    std::string compileIf(const std::shared_ptr<ASTNode>& node);
    std::string compileWhile(const std::shared_ptr<ASTNode>& node);
    std::string compileFor(const std::shared_ptr<ASTNode>& node);
    std::string compileBreak(const std::shared_ptr<ASTNode>& node);
    std::string compileContinue(const std::shared_ptr<ASTNode>& node);

    std::string compileExpression(const std::shared_ptr<ASTNode>& node, const std::string& targetReg = "%rax");
    std::string compileBinaryExpr(const std::shared_ptr<ASTNode>& node, const std::string& targetReg = "%rax");
    std::string compileUnaryExpr(const std::shared_ptr<ASTNode>& node, const std::string& targetReg = "%rax");
    std::string compileLiteral(const std::shared_ptr<ASTNode>& node);
    std::string compileIdentifier(const std::shared_ptr<ASTNode>& node, const std::string& targetReg = "%rax");
    std::string compileCallExpr(const std::shared_ptr<ASTNode>& node);

    int allocateLocal(const std::string& name, int size = 8); // default 8 bytes for int/ptr

    std::unordered_map<std::string, FunctionSymbol> functions;
    FunctionSymbol* currentFunction;
    int localOffset; // current stack offset for locals
    std::ostringstream bss;
    std::ostringstream data;
    std::ostringstream rodata;

    int str_idx;
};
