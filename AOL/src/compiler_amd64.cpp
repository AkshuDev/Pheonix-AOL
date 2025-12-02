#include "compiler_amd64.hpp"
#include <sstream>
#include <iostream>

Compiler_Amd64::Compiler_Amd64() : currentFunction(nullptr), localOffset(0) {}

std::string Compiler_Amd64::compile(const std::shared_ptr<ASTNode>& program) {
    if (!program) {
        return "";
    }
    std::ostringstream out;
    std::ostringstream tsec;

    bss << ":section .bss\n:align 8\n";
    data << ":section .data\n:align 8\n";
    rodata << ":section .rodata\n:align 8\n";

    tsec << compileProgram(program);
    out << rodata.str();
    out << data.str();
    out << bss.str();
    out << tsec.str();
    return out.str();
}

std::string Compiler_Amd64::compileProgram(const std::shared_ptr<ASTNode>& node) {
    std::ostringstream out;

    out << ":section .text\n:align 16\n:global __aol_main__\n\n";
    out << "__aol_main__:\n";
    out << "\tmov %rax, %rsp\n\tmov %rbx, %rdi\n"; // argc -> rax, argv -> rbx
    out << "\tcall $main\n";

    for (auto& child : node->children)
        out << compileStatement(child, "%rax");

    return out.str();
}

std::string Compiler_Amd64::compileStatement(const std::shared_ptr<ASTNode>& node, const std::string& targetReg) {
    if (!node) return "";

    switch (node->type) {
        case ASTNodeType::FunctionDecl: return compileFunction(node);
        case ASTNodeType::VariableDecl: return compileVariableDecl(node, targetReg);
        case ASTNodeType::ReturnStmt:   return compileReturn(node, targetReg);
        case ASTNodeType::IfStmt:       return compileIf(node);
        case ASTNodeType::WhileStmt:    return compileWhile(node);
        case ASTNodeType::ForStmt:      return compileFor(node);
        case ASTNodeType::BreakStmt:    return compileBreak(node);
        case ASTNodeType::ContinueStmt: return compileContinue(node);
        default:                        return compileExpression(node, targetReg) + "\n";
    }
}

std::string Compiler_Amd64::compileFunction(const std::shared_ptr<ASTNode>& node) {
    std::ostringstream out;

    FunctionSymbol func;
    func.name = node->name;
    func.stackSize = 0;
    func.body = node;
    currentFunction = &func;
    localOffset = 0;

    if (node->params.size() > 0) {
        // Params
        int offset = 0;
        for (auto& param : node->params) {
            func.params.push_back(
                (VariableInfo){
                    .name = param->name,
                    .offset = offset,
                    .size = 8 // Default
                }
            );
            func.stackSize += 8;
        }
    }

    functions[func.name] = func;

    out << ".func " << node->name << "\n";
    out << "\tpush %rbp\n\tpush 0\n\tmov %rbp, %rsp\n";

    // Compile all statements in the function
    for (auto& stmt : node->children)
        out << "\t" << compileStatement(stmt, "%rax");

    out << "\tpop %rbx\n\tpop %rbp\n";
    out << "\tret\n.endfunc\n\n";

    currentFunction = nullptr;
    return out.str();
}

std::string Compiler_Amd64::compileVariableDecl(const std::shared_ptr<ASTNode>& node, const std::string& targetReg) {
    std::ostringstream out;

    if (!currentFunction) {
        out << "\t// Warning: variable " << node->name << " declared outside function\n";
        return out.str();
    }

    int offset = allocateLocal(node->name, 8); // default 8 bytes
    if (!node->children.empty()) {
        out << "\tmov [%rbp - " << offset << "], " << compileExpression(node->children[0], targetReg) << "\n";
    } else {
        out << "\t// var " << node->name << " uninitialized at offset " << offset << "\n";
    }

    return out.str();
}

std::string Compiler_Amd64::compileReturn(const std::shared_ptr<ASTNode>& node, const std::string& targetReg) {
    std::ostringstream out;
    if (!node->children.empty()) {
        out << compileExpression(node->children[0], targetReg) << "\n";
    }
    out << "\tret\n";
    return out.str();
}

std::string Compiler_Amd64::compileIf(const std::shared_ptr<ASTNode>& node) {
    std::ostringstream out;
    std::string ifLabel = "__aol_if__";
    std::string elseLabel = "__aol_else__";
    std::string endLabel = "__aol_endif__";

    out << ifLabel << ":\n";
    out << "\tcmp " << compileExpression(node->children[0], "%rax") << ", 0\n";
    out << "\tje " << elseLabel << "\n";

    for (size_t i = 1; i < node->children.size(); ++i)
        out << compileStatement(node->children[i]);

    out << elseLabel << ":\n";
    out << endLabel << ":\n";
    return out.str();
}

std::string Compiler_Amd64::compileWhile(const std::shared_ptr<ASTNode>& node) {
    std::ostringstream out;
    std::string startLabel = "__aol_while__";
    std::string endLabel   = "__aol_while_end__";

    out << startLabel << ":\n";
    out << "\tcmp " << compileExpression(node->children[0], "%rax") << ", 0\n";
    out << "\tje " << endLabel << "\n";

    for (size_t i = 1; i < node->children.size(); ++i)
        out << compileStatement(node->children[i]);

    out << "\tjmp " << startLabel << "\n";
    out << endLabel << ":\n";
    return out.str();
}

std::string Compiler_Amd64::compileFor(const std::shared_ptr<ASTNode>& node) {
    std::ostringstream out;
    if (node->children.size() < 4) return "\t// malformed for loop\n";

    out << compileStatement(node->children[0]); // init
    std::string startLabel = "__aol_for__";
    std::string endLabel   = "__aol_for_end__";

    out << startLabel << ":\n";
    out << "\tcmp " << compileExpression(node->children[1], "%rax") << ", 0\n";
    out << "\tje " << endLabel << "\n";

    out << compileStatement(node->children[3]); // body
    out << compileStatement(node->children[2]); // increment
    out << "\tjmp " << startLabel << "\n";
    out << endLabel << ":\n";
    return out.str();
}

std::string Compiler_Amd64::compileBreak(const std::shared_ptr<ASTNode>&) {
    return "\t// break\n";
}

std::string Compiler_Amd64::compileContinue(const std::shared_ptr<ASTNode>&) {
    return "\t// continue\n";
}

std::string Compiler_Amd64::compileExpression(const std::shared_ptr<ASTNode>& node, const std::string& targetReg) {
    if (!node) return "";
    switch (node->type) {
        case ASTNodeType::BinaryExpr: return compileBinaryExpr(node, targetReg);
        case ASTNodeType::UnaryExpr:  return compileUnaryExpr(node, targetReg);
        case ASTNodeType::Literal:    return compileLiteral(node);
        case ASTNodeType::Identifier: return compileIdentifier(node, targetReg);
        case ASTNodeType::CallExpr:   return compileCallExpr(node);
        default: return "";
    }
}

std::string Compiler_Amd64::compileBinaryExpr(const std::shared_ptr<ASTNode>& node, const std::string& targetReg) {
    std::ostringstream out;
    out << compileExpression(node->children[0], targetReg) << " " << node->name << " " << compileExpression(node->children[1], targetReg);
    return out.str();
}

std::string Compiler_Amd64::compileUnaryExpr(const std::shared_ptr<ASTNode>& node, const std::string& targetReg) {
    return node->name + compileExpression(node->children[0], targetReg);
}

std::string Compiler_Amd64::compileLiteral(const std::shared_ptr<ASTNode>& node) {
    return node->value;
}

std::string Compiler_Amd64::compileIdentifier(const std::shared_ptr<ASTNode>& node, const std::string& targetReg) {
    // Find variable in locals or parameters
    if (!currentFunction) return node->name;
    (void)targetReg;

    for (auto& var : currentFunction->locals)
        if (var.name == node->name)
            return "[" + std::string("%rbp") + " - " + std::to_string(var.offset) + "]";

    for (auto& param : currentFunction->params)
        if (param.name == node->name)
            return "[" + std::string("%rbp") + " + " + std::to_string(param.offset) + "]";

    return node->name;
}

std::string Compiler_Amd64::compileCallExpr(const std::shared_ptr<ASTNode>& node) {
    std::ostringstream out;
    out << "\tcall $" << compileIdentifier(node->children[0]) << "\n";
    return out.str();
}

int Compiler_Amd64::allocateLocal(const std::string& name, int size) {
    if (!currentFunction) return 0;
    localOffset += size;
    currentFunction->locals.push_back({name, localOffset, size});
    return localOffset;
}

