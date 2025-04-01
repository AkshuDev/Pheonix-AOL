#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <string>
#include <map>
#include <variant>
#include <vector>
#include "tokens.h"

#include "errorHandler.h"

class Evaluator_A {
public:
    Evaluator_A(const std::vector<AOL_TOKEN::Token>& tokens);
    void evaluate();

    float factor();

    // Getter for variables map
    int getVariable(const std::string& varName) const {
        auto it = variables.find(varName);
        if (it != variables.end()) {
            if (auto intValue = std::get_if<int>(&it->second)) {
                return *intValue;
            } else {
                throw std::runtime_error("Variable is not an integer");
            }
        }
        throw std::runtime_error("Variable not found");
    }

    void printVariables() const {
        for (const auto& var : variables) {
            std::cout << var.first << " = ";
            std::visit([](const auto& value) { std::cout << value; }, var.second);
            std::cout << std::endl;
        }
    }

private:
    std::vector<AOL_TOKEN::Token> tokens;
    std::map<std::string, std::variant<int, float, char, std::string, bool>> variables;  // Store variable names and their integer values
    std::map<std::string, int> var_codespace; // Store the codespaces of vars
    std::vector<int> codespaces; // Available codespaces
    int current_codespace = 0; // Current Codespace
    size_t pos;

    AOL_TOKEN::Token currentToken();
    AOL_TOKEN::Token nextToken();
    void parseAssignment();
    float parseExponentiation();
    float parseMultiplicationDivision();
    float parseAdditionSubtraction();
    float parseExpression();
    int parseNumber();
    void executeAssignment(const std::string& varName, float value);
    char evalChar();
    std::string evalString();
    bool evalCondition();
};

#endif
