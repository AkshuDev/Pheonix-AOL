// Std includes
#include <iostream>
#include <cstring>
#include <string>
#include <cctype>
#include <vector>
// Project includes
#include <lexer.h>
#include <aol.h>
#include <basicio.h>
#include <errorHandler.h>
#include <eval.h>

void Cleanup() {
    return;
}

int main(int argc, char* argv[]) {
    std::vector<std::string> files;
    std::string path; // To build the current path

    const char* code_c;
    bool inQuotes = false; // To track if we are inside quotes

    bool Token_Print = false;

    if (argc > 1) {
        for (int arg = 1; arg < argc; arg++) {
            std::string currentArg = argv[arg];
            // Check if the argument contains a quote
            if (currentArg[0] == '\"') {
                // Process quoted arguments
                for (int i = arg; i < argc; i++) {
                    std::string currentArg_ = argv[i];
                    // Check for quotes
                    size_t startQuote = currentArg_.find('"');
                    size_t endQuote = currentArg_.rfind('"');

                    if (startQuote != std::string::npos) {
                        // If we find a starting quote, we toggle the inQuotes flag
                        inQuotes = !inQuotes;
                        // Remove the quote from the path
                        currentArg_.erase(startQuote, 1);
                    }

                    // Append the current argument to the path
                    path += currentArg_;

                    // If we find an ending quote and we were in quotes, break the loop
                    if (endQuote != std::string::npos && inQuotes) {
                        path.erase(endQuote - 1, 1); // Remove the ending quote
                        files.push_back(path); // Add the completed path to the files vector
                        path.clear(); // Clear the path for the next argument
                        break; // Break out of the inner loop
                    }

                    // If we are not in quotes, we can add a space to separate arguments
                    if (!inQuotes) {
                        path += " ";
                    }
                }
            } else if (currentArg == "-debug:false") {
                Debug = false;
            } else if (currentArg == "-debug:true") {
                Debug = true;
            } else if (currentArg == "-debugout" && arg + 1 < argc) {
                DebugOut = std::stoi(argv[arg + 1]);
                if (DebugOut > 4 || DebugOut < 0) {
                    DebugOut = 1;
                    Debug_("Argument Warning -> AOL", "[-debugout] command can have a maximum number of [4]. Set to 1.", "Warning", "DEBUG", true, 1);
                }
            } else if (currentArg == "-token") {
                Token_Print = true;
            } else if (currentArg[0] == '-') {
                Debug_("Argument Error -> AOL", "Unrecognized option: " + currentArg, "Argument Error", std::to_string(static_cast<int>(ReturnCodes::Argument::InvalidArgument)), true, 1);
                return static_cast<int>(ReturnCodes::Argument::InvalidArgument); // Exit with error
            } else {
                // If the argument is not quoted, just add it to the path
                files.push_back(argv[arg]);
            }
        }

        // Check file extensions
        for (const auto& file : files) {
            if (file.size() < 4 || (file.substr(file.size() - 4) != ".aol" && file.substr(file.size() - 2) != ".o")) {
                Debug_("Argument Error -> AOL", "Unsupported file extension for file: " + file, "Argument Error", std::to_string(static_cast<int>(ReturnCodes::Argument::InvalidArgument)), true, 1);
                return static_cast<int>(ReturnCodes::Argument::InvalidArgument); // Exit with an error code
            }
        }

    } else {
        mode = 1;
    }

    switch (mode){
        case 2:
            code_c = FileIo(files[0].c_str(), FileMode::ReadNormal, 0, nullptr);
            if (strstr(code_c, ERROR_BASICIO) != nullptr) {
                Debug_("File Operation Error", "[FileIo@basicio.h@basicio.dll] has failed!\n\tPath of file: " + files[0], "Unknown", std::to_string(static_cast<int>(ReturnCodes::General::Unknown)), true, 1);
                return static_cast<int>(ReturnCodes::General::Unknown);
            }
        default:
            code_c = FileIo(files[0].c_str(), FileMode::ReadNormal, 0, nullptr);
            if (strstr(code_c, ERROR_BASICIO) != nullptr) {
                Debug_("File Operation Error", "[FileIo@basicio.h@basicio.dll] has failed!\n\tPath of file: " + files[0], "Unknown", std::to_string(static_cast<int>(ReturnCodes::General::Unknown)), true, 1);
                return static_cast<int>(ReturnCodes::General::Unknown);
            }
    }

    std::string code(code_c);

    Lexer_A lexer(code);
    std::vector<AOL_TOKEN::Token> tokens = lexer.tokenize();

    if (Debug && Token_Print){
        for (const auto& token : tokens) {
            int type = static_cast<int>(token.type);
            std::string typeS;

            switch (type) {
                case static_cast<int>(AOL_TOKEN::TokenType::ASSIGN):            typeS = "ASSIGN"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::INT):               typeS = "INT"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::CHAR):              typeS = "CHAR"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::STRING):            typeS = "STRING"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::FLOAT):             typeS = "FLOAT"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::VAR):               typeS = "VAR"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::ENDLINE):           typeS = "ENDLINE"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::IDENTIFIER):        typeS = "IDENTIFIER"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::INT_VAR):           typeS = "INT_VAR"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::CHAR_VAR):          typeS = "CHAR_VAR"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::STRING_VAR):        typeS = "STRING_VAR"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::FLOAT_VAR):         typeS = "FLOAT_VAR"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::ADD):               typeS = "ADD"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::SUB):               typeS = "SUB"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::MULT):              typeS = "MULT"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::DIV):               typeS = "DIV"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::FDIV):              typeS = "FDIV"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::EXPO):              typeS = "EXPO"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::MOD):               typeS = "MOD"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::LPAR):              typeS = "LPAR"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::RPAR):              typeS = "RPAR"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::XOR):               typeS = "XOR"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::AND):               typeS = "AND"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::OR):                typeS = "OR"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::NOT):               typeS = "NOT"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::GREATER_THAN):      typeS = "GREATER_THAN"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::GREATER_OR_EQUALS): typeS = "GREATER_EQUALS"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::LESS_THAN):         typeS = "LESS_THAN"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::LESS_OR_EQUALS):    typeS = "LESS_EQUALS"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::EQUALS):            typeS = "EQUALS"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::TRUE_):             typeS = "TRUE"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::BOOL):              typeS = "BOOL"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::BOOL_VAR):          typeS = "BOOL_VAR"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::FALSE_):            typeS = "FALSE"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::IF):            typeS = "IF"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::ELSE):            typeS = "ELSE"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::ELSE_IF):            typeS = "ELSE_IF"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::CODE_BLOCK_OPEN):            typeS = "CODE_BLOCK_OPEN"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::CODE_BLOCK_CLOSE):            typeS = "CODE_BLOCK_CLOSE"; break;
                case static_cast<int>(AOL_TOKEN::TokenType::UNKNOWN):           typeS = "UNKNOWN"; break;
                default: typeS = "UNKNOWN"; break;
            }

            Debug_("Token: " +  typeS + ", Value: " + token.value + "\n", "", "", "", false, 0);
        }
    }

    Evaluator_A eval(tokens);
    eval.evaluate();

    if (Debug){
        Debug_("Variable values after evaluation:\n", "", "", "", false, 0);
        eval.printVariables();
    }

    Cleanup();
    return 0;
}