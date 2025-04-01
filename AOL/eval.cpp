#include <eval.h>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <typeinfo>
#include <stack>
#include <unordered_map>

Evaluator_A::Evaluator_A(const std::vector<AOL_TOKEN::Token>& tokens)
    : tokens(tokens), pos(0) {}

AOL_TOKEN::Token Evaluator_A::currentToken() {
    if (pos < tokens.size()) {
        return tokens[pos];
    }
    return {AOL_TOKEN::TokenType::UNKNOWN, ""}; // End of tokens
}

AOL_TOKEN::Token Evaluator_A::nextToken() {
    if (pos < tokens.size()) {
        pos++;
    }
    return currentToken();
}

void Evaluator_A::evaluate() {
    while (pos < tokens.size()) {
        AOL_TOKEN::Token token = tokens[pos];

        if (token.type == AOL_TOKEN::TokenType::INT_VAR || token.type == AOL_TOKEN::TokenType::FLOAT_VAR) {
            bool isFloat = token.type == AOL_TOKEN::TokenType::FLOAT_VAR ? true : false;
            pos++; // Skip the 'int' keyword

            if (pos < tokens.size() && tokens[pos].type == AOL_TOKEN::TokenType::IDENTIFIER) {
                std::string varName = tokens[pos].value;
                pos++;

                if (pos < tokens.size() && tokens[pos].type == AOL_TOKEN::TokenType::ASSIGN) {
                    pos++; // Skip '='

                    float value = parseExpression(); // Get the value
                    if (!isFloat) {
                        variables[varName] = static_cast<int>(value);
                    } else {
                        variables[varName] = value;
                    }
                    var_codespace[varName] = current_codespace;
                } else {
                    throwError("Expected '=' after variable name.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
                }
            } else {
                throwError("Expected variable name after 'char'.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }
        } else if (token.type == AOL_TOKEN::TokenType::STRING_VAR) {
            pos++; // Skip 'string' keyword

            if (pos < tokens.size() && tokens[pos].type == AOL_TOKEN::TokenType::IDENTIFIER) {
                std::string varName = tokens[pos].value;
                pos++;

                if (pos < tokens.size() && tokens[pos].type == AOL_TOKEN::TokenType::ASSIGN) {
                    pos++; // Skip '='

                    std::string value = evalString(); // Get the string value
                    variables[varName] = value;
                } else {
                    throwError("Expected '=' after string variable name.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
                }
            } else {
                throwError("Expected variable name after 'string'.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }
        } else if (token.type == AOL_TOKEN::TokenType::CHAR_VAR) {
            pos++; // Skip 'char' keyword

            if (pos < tokens.size() && tokens[pos].type == AOL_TOKEN::TokenType::IDENTIFIER) {
                std::string varName = tokens[pos].value;
                pos++;

                if (pos < tokens.size() && tokens[pos].type == AOL_TOKEN::TokenType::ASSIGN) {
                    pos++; // Skip '='

                    char value = evalChar(); // Get the char value
                    variables[varName] = value;
                } else {
                    throwError("Expected '=' after char variable name.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
                }
            } else {
                throwError("Expected variable name after 'char'.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }
        } else if (token.type == AOL_TOKEN::TokenType::BOOL_VAR) {
            pos++; // Skip 'bool' keyword

            if (pos < tokens.size() && tokens[pos].type == AOL_TOKEN::TokenType::IDENTIFIER) {
                std::string varName = tokens[pos].value;
                pos++;

                if (pos < tokens.size() && tokens[pos].type == AOL_TOKEN::TokenType::ASSIGN) {
                    pos++; // Skip '='

                    bool value = evalCondition(); // Get the bool value
                    variables[varName] = value;
                } else {
                    throwError("Expected '=' after bool variable name.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
                }
            } else {
                throwError("Expected variable name after 'bool'.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }
        } else if (token.type == AOL_TOKEN::TokenType::IDENTIFIER) {
            // Check if the variable has been assigned a value or not
            if (variables.find(token.value) == variables.end()) {
                throwError("Undefined variable: " + token.value, "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }

            std::string identifier = token.value;

            pos++; // Consume identifier

            if (token.type == AOL_TOKEN::TokenType::ASSIGN) {
                pos++; // Consume assignment
                std::variant<int, float, char, std::string, bool> value;

                if (std::holds_alternative<bool>(variables[identifier])) {
                    value = evalCondition();
                }
                else if (std::holds_alternative<std::string>(variables[identifier])) {
                    value = evalString();
                }
                else if (std::holds_alternative<char>(variables[identifier])) {
                    value = evalChar();
                }
                else if (std::holds_alternative<int>(variables[identifier]) || std::holds_alternative<float>(variables[identifier])) {
                    value = factor();
                }

                if (std::holds_alternative<int>(variables[identifier])) {
                    if (std::holds_alternative<float>(value)) {
                        value = static_cast<int>(std::get<float>(value));
                    }
                }

                variables[identifier] = std::visit([](auto&& val) -> std::variant<int, float, char, std::string, bool> { return val; }, value);
            }

        } else if (token.type == AOL_TOKEN::TokenType::CODE_BLOCK_OPEN) {
            current_codespace += 1;
        } else if (token.type == AOL_TOKEN::TokenType::CODE_BLOCK_CLOSE) {
            current_codespace -= 1;
        } else {
            nextToken();
            continue;
        }
    }
}

float Evaluator_A::factor() {
    if (pos >= tokens.size()) {
        throwError("Unexpected end of input in factor()", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
    }

    AOL_TOKEN::Token token = tokens[pos];

    // Case 1: Number (integer or float)
    if (token.type == AOL_TOKEN::TokenType::INT || token.type == AOL_TOKEN::TokenType::FLOAT) {
        nextToken(); // move past the number
        return std::stof(token.value); // Return as an integer (or float if necessary)
    }

    // Case 2: Variable (Identifier)
    if (token.type == AOL_TOKEN::TokenType::IDENTIFIER) {
        std::string varName = token.value;
        nextToken();  // Move past the identifier

        // Check if the variable has been assigned a value
        if (variables.find(varName) != variables.end()) {
            if (var_codespace.find(varName) == var_codespace.end()){
                throwError("Variable [" + varName + "] Codespace not found!", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }

            if (var_codespace.find(varName) != current_codespace || var_codespace.find(varName) < current_codespace) {
                throwError("Variable [" + varName + "] is undefined!", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }

            if (std::holds_alternative<std::string>(variables[varName]) || std::holds_alternative<char>(variables[varName])) {
                    throwError("Cannot evaluate a [int] variable with identifier [" + varName + "] of type [string/char].", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }
            // Return the value stored in the variable
            if (std::holds_alternative<int>(variables[varName])) {
                return static_cast<float>(std::get<int>(variables[varName]));
            } else if (std::holds_alternative<float>(variables[varName])) {
                return std::get<float>(variables[varName]);
            } else {
                throwError("Cannot evaluate variable [" + varName + "] as float.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }
        } else {
            throwError("Undefined variable: " + varName, "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
        }
    }

    // Case 3: Parentheses -> ( expression )
    if (token.type == AOL_TOKEN::TokenType::LPAR) {
        nextToken(); // Move past '('
        float result = parseExpression(); // Recursively evaluate inside parentheses

        if (pos >= tokens.size() || tokens[pos].type != AOL_TOKEN::TokenType::RPAR) {
            throwError("Expected closing ')'", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
        } else {
            nextToken(); // Move past ')'
        }
        return result;
    }

    throwError("Unexpected token in factor(): " + token.value, "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
    return 0.0f;
}

float Evaluator_A::parseExpression() {
    return parseAdditionSubtraction(); // Start parsing addition and subtraction
}

float Evaluator_A::parseMultiplicationDivision() {
    float left = parseExponentiation();

    while (pos < tokens.size() && (tokens[pos].type == AOL_TOKEN::TokenType::MULT || tokens[pos].type == AOL_TOKEN::TokenType::DIV || tokens[pos].type == AOL_TOKEN::TokenType::FDIV || tokens[pos].type == AOL_TOKEN::TokenType::MOD)) {
        AOL_TOKEN::TokenType op = tokens[pos].type;
        pos++;

        float right = parseExponentiation();

        if (op == AOL_TOKEN::TokenType::MULT) left *= right;
        else if (op == AOL_TOKEN::TokenType::DIV) {
            if (right == 0) throwError("Division by zero.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            left /= right;
        }
        else if (op == AOL_TOKEN::TokenType::FDIV) {
            if (right == 0) throwError("Integer division by zero.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            left = std::floor(left / right);
        }
        else if (op == AOL_TOKEN::TokenType::MOD) {
            if (right == 0) throwError("Modulo by zero.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            left = std::fmod(left, right);
        }
    }
    return left;
}

float Evaluator_A::parseAdditionSubtraction() {
    float left = parseMultiplicationDivision();

    while (pos < tokens.size() && (tokens[pos].type == AOL_TOKEN::TokenType::ADD || tokens[pos].type == AOL_TOKEN::TokenType::SUB)) {
        AOL_TOKEN::TokenType op = tokens[pos].type;
        pos++;

        float right = parseMultiplicationDivision();

        if (op == AOL_TOKEN::TokenType::ADD) left += right;
        else if (op == AOL_TOKEN::TokenType::SUB) left -= right;
    }
    return left;
}

float Evaluator_A::parseExponentiation() {
    float left = factor();

    while (pos < tokens.size() && tokens[pos].type == AOL_TOKEN::TokenType::EXPO) {
        AOL_TOKEN::Token op = tokens[pos];
        pos++;

        float right = factor();
        left = std::pow(left, right);
    }
    return left;
}

// Parsing a complete expression (e.g., 123 + 2)
int Evaluator_A::parseNumber() {
    AOL_TOKEN::Token numToken = currentToken();
    if (numToken.type == AOL_TOKEN::TokenType::UNKNOWN) {
        throwError("Invalid number.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
    }

    int value = std::stoi(numToken.value);  // Convert string to int
    nextToken();  // Move to the next token
    return value;
}

void Evaluator_A::parseAssignment() {
    // Expect IDENTIFIER for variable name
    AOL_TOKEN::Token varToken = nextToken();
    if (varToken.type != AOL_TOKEN::TokenType::IDENTIFIER) {
        throwError("Expected variable name for assignment.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
    }

    std::string varName = varToken.value;

    // Expect ASSIGN sign '='
    AOL_TOKEN::Token assignOp = nextToken();
    if (assignOp.type != AOL_TOKEN::TokenType::ASSIGN) {
        throwError("Expected '=' after variable.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
    }

    int value = parseNumber();  // Parse the value of the variable

    // Store the variable value
    executeAssignment(varName, value);
}

void Evaluator_A::executeAssignment(const std::string& varName, float value) {
    if (variables.find(varName) != variables.end() && std::holds_alternative<int>(variables[varName])) {
        variables[varName] = static_cast<int>(value);
    } else {
        variables[varName] = value;
    }
    std::cout << "Assigned " << value << " to variable " << varName << std::endl;
}

char Evaluator_A::evalChar() {
    AOL_TOKEN::Token token = currentToken();
    char c;

    if (token.type != AOL_TOKEN::TokenType::CHAR && token.type != AOL_TOKEN::TokenType::IDENTIFIER) {
        throwError("Expected a character literal or a variable name.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
    }

    if (token.type == AOL_TOKEN::TokenType::CHAR) {
        c = token.value[0];
    } else { // token.type == AOL_TOKEN::TokenType::IDENTIFIER
        if (variables.find(token.value) != variables.end()) {
            if (!std::holds_alternative<char>(variables[token.value])) {
                throwError("Variable " + token.value + " is not a char.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }
            c = std::get<char>(variables[token.value]);
        } else {
            throwError("Undefined variable: " + token.value, "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
        }
    }

    nextToken(); // Consume token
    return c;
}

// Evaluates a string literal or string variable
std::string Evaluator_A::evalString() {
    AOL_TOKEN::Token token = currentToken();
    std::string s;

    if (token.type != AOL_TOKEN::TokenType::STRING && token.type != AOL_TOKEN::TokenType::IDENTIFIER) {
        throwError("Expected a string literal or variable.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
    }

    if (token.type == AOL_TOKEN::TokenType::STRING) {
        s = token.value;
    } else { // token.type == AOL_TOKEN::TokenType::IDENTIFIER
        if (variables.find(token.value) != variables.end()) {
            if (std::holds_alternative<std::string>(variables[token.value])){
                s = std::get<std::string>(variables[token.value]);
            } else if (std::holds_alternative<char>(variables[token.value])) {
                s += std::get<char>(variables[token.value]);
            } else {
                throwError("Variable " + token.value + " is not a string or a character.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }
        } else {
            throwError("Undefined variable: " + token.value, "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
        }
    }

    token = nextToken(); // Consume token

    if (token.type == AOL_TOKEN::TokenType::ENDLINE) {
        return s;
    } else if (token.type == AOL_TOKEN::TokenType::ADD) {
        nextToken(); // Consume '+'
        s += evalString();
    } else if (token.type == AOL_TOKEN::TokenType::MULT) {
        nextToken(); // Consume '*'
        if (currentToken().type != AOL_TOKEN::TokenType::INT && currentToken().type != AOL_TOKEN::TokenType::IDENTIFIER) {
            throwError("Can only repeat string using an integer.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
        }
        int value;
        if (currentToken().type == AOL_TOKEN::TokenType::INT){
            value = parseNumber();
        } else {
            token = currentToken();

            if (variables.find(token.value) != variables.end()) {
                if (!std::holds_alternative<int>(variables[token.value])) {
                    throwError("Variable " + token.value + " is not a int.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
                }
                value = std::get<int>(variables[token.value]);
            } else {
                throwError("Undefined variable: " + token.value, "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }
        }
        std::string repeated;
        for (int i = 0; i < value; i++) repeated += s;
        s = repeated;
    }

    return s;
}

// Evaluate a conditional expression
bool Evaluator_A::evalCondition() {
    AOL_TOKEN::Token token = currentToken();
    bool result;

    std::variant<int, float> varValue = 0; // Initialize default values
    std::variant<int, float> varValue2 = 0;

    // Handle logical NOT (!)
    if (token.type == AOL_TOKEN::TokenType::NOT) {
        nextToken();  // Move past '!'
        result = !evalCondition();  // Recursively evaluate the next condition
        return result;  // Immediately return the negated result
    }
    // Handle boolean values
    else if (token.type == AOL_TOKEN::TokenType::TRUE_) {
        result = true;
        nextToken();
        return result;
    } else if (token.type == AOL_TOKEN::TokenType::FALSE_) {
        result = false;
        nextToken();
        return result;
    }

    // Handle integer and float values
    if (token.type == AOL_TOKEN::TokenType::INT || token.type == AOL_TOKEN::TokenType::FLOAT) {
        if (token.type == AOL_TOKEN::TokenType::INT) {
            varValue = std::stoi(token.value);
        } else {
            varValue = std::stof(token.value);
        }
        nextToken();  // Move to next token

        // Ensure we have a valid operator after the number
        token = currentToken();
        if (token.type == AOL_TOKEN::TokenType::GREATER_THAN ||
            token.type == AOL_TOKEN::TokenType::GREATER_OR_EQUALS ||
            token.type == AOL_TOKEN::TokenType::LESS_THAN ||
            token.type == AOL_TOKEN::TokenType::LESS_OR_EQUALS) {

            AOL_TOKEN::TokenType op = token.type;
            nextToken();  // Move to right-hand side

            token = currentToken();
            if (token.type == AOL_TOKEN::TokenType::IDENTIFIER) {
                if (variables.find(token.value) != variables.end()) {
                    if (std::holds_alternative<int>(variables[token.value])) {
                        varValue2 = std::get<int>(variables[token.value]);
                    } else if (std::holds_alternative<float>(variables[token.value])) {
                        varValue2 = std::get<float>(variables[token.value]);
                    }
                }
            } else if (token.type == AOL_TOKEN::TokenType::INT) {
                varValue2 = std::stoi(token.value);
            } else if (token.type == AOL_TOKEN::TokenType::FLOAT) {
                varValue2 = std::stof(token.value);
            } else {
                throwError("Expected a number or variable after comparison operator.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }

            nextToken();  // Move past right-hand value

            // Perform the comparison
            switch (op) {
                case AOL_TOKEN::TokenType::GREATER_THAN:
                    result = std::get<float>(varValue) > std::get<float>(varValue2);
                    break;
                case AOL_TOKEN::TokenType::GREATER_OR_EQUALS:
                    result = std::get<float>(varValue) >= std::get<float>(varValue2);
                    break;
                case AOL_TOKEN::TokenType::LESS_THAN:
                    result = std::get<float>(varValue) < std::get<float>(varValue2);
                    break;
                case AOL_TOKEN::TokenType::LESS_OR_EQUALS:
                    result = std::get<float>(varValue) <= std::get<float>(varValue2);
                    break;
                default:
                    throwError("Unexpected comparison operator.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }

            return result;  // Immediately return result after comparison
        }
    }

    // Handle identifiers (variables)
    if (token.type == AOL_TOKEN::TokenType::IDENTIFIER) {
        if (variables.find(token.value) != variables.end()) {
            if (std::holds_alternative<int>(variables[token.value])) {
                varValue = std::get<int>(variables[token.value]);
            } else if (std::holds_alternative<float>(variables[token.value])) {
                varValue = std::get<float>(variables[token.value]);
            } else if (std::holds_alternative<bool>(variables[token.value])) {
                result = std::get<bool>(variables[token.value]);
                nextToken();
                return result;
            } else {
                throwError("Variable " + token.value + " is not a valid boolean or number.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }
        } else {
            throwError("Undefined variable: " + token.value, "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
        }
        nextToken();
    }

    // Handle logical operators (&&, ||, ==, !=)
    while (true) {
        token = currentToken();

        if (token.type == AOL_TOKEN::TokenType::AND ||
            token.type == AOL_TOKEN::TokenType::OR ||
            token.type == AOL_TOKEN::TokenType::EQUALS ||
            token.type == AOL_TOKEN::TokenType::NOT_EQUALS) {

            AOL_TOKEN::TokenType op = token.type;
            nextToken();  // Consume operator

            bool rightResult = evalCondition();  // Recursively evaluate right-hand expression

            switch (op) {
                case AOL_TOKEN::TokenType::AND:
                    result = result && rightResult;
                    break;
                case AOL_TOKEN::TokenType::OR:
                    result = result || rightResult;
                    break;
                case AOL_TOKEN::TokenType::EQUALS:
                    result = (result == rightResult);
                    break;
                case AOL_TOKEN::TokenType::NOT_EQUALS:
                    result = (result != rightResult);
                    break;
                default:
                    throwError("Unexpected operator in boolean expression.", "", "Evaluation Error", "NO ERROR CODE", true, 1, ColorCodes::RED);
            }
        } else {
            break;  // No more operators, exit loop
        }
    }

    return result;
}
