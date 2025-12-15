#include <compiler_amd64.hpp>
#include <sstream>
#include <cctype>
#include <iostream>
#include <algorithm>

Compiler_Amd64::Compiler_Amd64() : currentFunction(nullptr), localOffset(0) {}

std::string Compiler_Amd64::compile(const std::shared_ptr<ASTNode>& program) {
    if (!program) {
        return "";
    }

    bss.str(""); bss.clear();
    data.str(""); data.clear();
    rodata.str(""); rodata.clear();

    std::ostringstream out;
    std::ostringstream tsec;

    bss << "\t:align 8\n:section .bss\n";
    data << "\t:align 8\n:section .data\n";
    rodata << "\t:align 8\n:section .rodata\n";
    rodata << "\t__aol_entry_dbg!ubyte[] = \"DBG: Entry!\", 10\n";

    tsec << compileProgram(program);
    out << rodata.str();
    out << data.str();
    out << bss.str();
    out << tsec.str();
    return out.str();
}

std::string Compiler_Amd64::compileProgram(const std::shared_ptr<ASTNode>& node) {
    std::ostringstream out;

    out << "\t:align 16\n:section .text\n\t:global __aol_main__\n\n";
    out << "__aol_main__:\n";
    out << "\tmov %rdi, %rsp\n\tmov %rsi, %rdi\n"; // argc -> rax, argv -> rbx
    out << "\tlea %rdi, [__aol_entry_dbg]\n\tmov %rsi, 12\n\tcall __aol_print\n";
    out << "\tcall $main\n";
    out << "\tjmp __aol_exit\n\tret\n";
    out << "__aol_print:\n";
    out << "\tmov %rsi, %rdi\n\tmov %rdx, %rsi\n\tmov %rax, 1\n\tmov %rdi, 1\n";
    out << "\tsyscall\n\tret\n\n";

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
        case ASTNodeType::Literal:      return compileLiteral(node);
        case ASTNodeType::CallExpr:     return compileCallExpr(node);
        default:                        return compileExpression(node, targetReg) + "\n";
    }
}

std::string Compiler_Amd64::compileFunction(const std::shared_ptr<ASTNode>& node) {
    std::ostringstream out;
    std::ostringstream func_s;

    FunctionSymbol func;
    func.name = node->name;
    func.stackSize = 0;
    func.body = node;
    currentFunction = &func;
    localOffset = 0;

    // Assign parameter offsets (System V AMD64 ABI: rdi, rsi, rdx, rcx, r8, r9, rest on stack)
    const std::vector<std::string> paramRegs = {"%rdi","%rsi","%rdx","%rcx","%r8","%r9"};
    int stackParamOffset = 16; // Start of first stack param (after saved rbp + return addr)
    
    for (size_t i = 0; i < node->params.size(); ++i) {
        VariableInfo v;
        v.name = node->params[i]->name;
        v.size = 8; // default
        if (i < paramRegs.size()) {
            v.offset = 0; // Mark as register-passed
            v.reg = paramRegs[i];
        } else {
            v.offset = stackParamOffset;
            v.reg = ""; 
            stackParamOffset += 8;
        }
        func.params.push_back(v);
    }

    functions[func.name] = func;

    // Function prologue
    func_s << ".func " << node->name << "\n";
    func_s << "\tpush %rbp\n";
    func_s << "\tmov %rbp, %rsp\n";

    // Move register params into stack locals for uniform access
    for (auto& param : func.params) {
        if (!param.reg.empty()) {
            int offset = allocateLocal(param.name, param.size);
            out << "\tmov [%rbp - " << offset << "], " << param.reg << "\n";
        }
    }

    // Compile statements
    for (auto& stmt : node->children)
        out << compileStatement(stmt, "%rax");

    // Function epilogue
    out << "\tmov %rsp, %rbp\n";
    out << "\tpop %rbp\n";
    out << "\tret\n";
    out << ".endfunc\n\n";

    func_s << "\tsub %rsp, " << currentFunction->stackSize << "\n";
    func_s << out.str();

    currentFunction = nullptr;
    return func_s.str();
}

std::string Compiler_Amd64::compileCallExpr(const std::shared_ptr<ASTNode>& node) {
    if (functions.find(node->name) == functions.end()) {
        std::cerr << "Error: Unknown function '" << node->name << "' at line " << node->line << " col " << node->col << "\n";
        return "";
    }

    auto func = functions.at(node->name);
    std::ostringstream out;

    // Evaluate arguments
    std::vector<std::string> argRegs = {"%rdi","%rsi","%rdx","%rcx","%r8","%r9"};
    size_t nArgs = node->children.size();

    // Push stack args first (reverse-order)
    for (size_t i = nArgs; i-- > argRegs.size();) {
        out << "\tpush " << compileExpression(node->children[i], "%rax") << "\n";
    }

    // Move first 6 args into registers
    for (size_t i = 0; i < std::min(nArgs, argRegs.size()); ++i) {
        out << "\tmov " << compileExpression(node->children[i], "%rax") 
            << ", " << argRegs[i] << "\n";
    }

    // Final stuff
    out << "\tcall $" << node->name << "\n";

    if (nArgs > argRegs.size()) {
        out << "\tadd %rsp, " << (int64_t)(8 * (nArgs - argRegs.size())) << "\n";
    }

    // Return is in rax reg
    return out.str();
}

std::string Compiler_Amd64::compileLiteral(const std::shared_ptr<ASTNode>& node) {
    if (std::all_of(node->value.begin(), node->value.end(), [](unsigned char c){return std::isdigit(c);})) {
        return node->value;
    }
    rodata << "\tstr_" << str_idx << "!ubyte[] = \"" << node->value << "\"\n";
    std::ostringstream out;
    out << "str_" << str_idx;
    str_idx++;
    return out.str();
}

std::string Compiler_Amd64::compileReturn(const std::shared_ptr<ASTNode>& node, const std::string& targetReg) {
    std::ostringstream out;
    if (!node->children.empty()) {
        out << compileExpression(node->children[0], targetReg) << "\n";
    }
    out << "\tleave\n\tret\n";
    return out.str();
}

std::string Compiler_Amd64::compileVariableDecl(const std::shared_ptr<ASTNode>& node, const std::string& targetReg) {
    if (!currentFunction) return "";

    std::ostringstream out;
    int offset = allocateLocal(node->name, 8); // locals: negative offset

    if (!node->children.empty()) {
        std::string val = compileExpression(node->children[0], "no_reg");
        out << "\tmov [" << "%rbp - " << offset << "], " << val << "\n";
    } else {
        out << "\t// uninitialized var " << node->name << "\n";
        bss << "\t:res " << node->name << "!ubyte";
    }
    return out.str();
}

std::string Compiler_Amd64::compileIf(const std::shared_ptr<ASTNode>& node) {
    std::ostringstream out;
    std::string ifLabel = "__aol_if__";
    std::string elseLabel = "__aol_else__";
    std::string endLabel = "__aol_endif__";

    out << ifLabel << ":\n";
    out << "\tcmp " << compileExpression(node->children[0], "no_reg") << ", 0\n";
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
    out << "\tcmp " << compileExpression(node->children[0], "no_reg") << ", 0\n";
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
    out << "\tcmp " << compileExpression(node->children[1], "no_reg") << ", 0\n";
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
        case ASTNodeType::Literal: {
            if (targetReg == "no_reg") {
                return compileLiteral(node);
            }
            std::ostringstream out;
            out << "\tmov " << targetReg << ", " << compileLiteral(node) << "\n";
            return out.str();
        }
        case ASTNodeType::Identifier: return compileIdentifier(node, targetReg);
        case ASTNodeType::CallExpr: {
            if (targetReg == "%rax") return compileCallExpr(node);
            std::ostringstream out;
            out << compileCallExpr(node);
            out << "\tmov " << targetReg << ", %rax\n";
            return out.str();
        }
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

std::string Compiler_Amd64::compileIdentifier(const std::shared_ptr<ASTNode>& node, const std::string& targetReg) {
    if (!currentFunction) return node->name;

    // Check locals
    for (auto& var : currentFunction->locals) {
        if (var.name == node->name) {
            if (targetReg == "no_reg") {
                return "[" + std::string("%rbp") + " - " + std::to_string(var.offset) + "]\n";
            }
            return "\tmov " + targetReg + ", [" + std::string("%rbp") + " - " + std::to_string(var.offset) + "]\n";
        }
    }

    // Check parameters
    for (auto& param : currentFunction->params) {
        if (param.name == node->name) {
            if (!param.reg.empty()) 
                return param.reg; // use register directly
            else 
                return "[" + std::string("%rbp") + " + " + std::to_string(param.offset) + "]"; // stack
        }
    }

    return node->name; // fallback
}

int Compiler_Amd64::allocateLocal(const std::string& name, int size) {
    if (!currentFunction) return 0;
    localOffset += size;
    currentFunction->locals.push_back({name, localOffset, size, ""});
    currentFunction->stackSize += size;
    return localOffset;
}

