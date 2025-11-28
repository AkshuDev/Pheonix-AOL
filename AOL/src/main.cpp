#include <iostream>
#include <fstream>

#include <args.hpp>
#include <colors.hpp>
#include <lexer.hpp>

int main(int argc, char** argv) {
    ArgParser parser(argv[0]);

    parser.addOption("-o", "--output", "Specify output file name", true);
    parser.addOption("-v", "--verbose", "Enable verbose mode", false);
    parser.addOption("-c", "--compile-only", "Compile but do not JIT", false);
    parser.addOption("", "--lexout", "Stop after lexing and print all tokens", false);

    bool showHelp = false;
    if (!parser.parse(argc, argv, showHelp)) {
        std::cout << "\nRun with --help\n";
        return 1;
    }

    if (showHelp) {
        std::cout << parser.help();
        return 0;
    }

    auto files = parser.positional();
    if (files.empty()) {
        std::cout << Color::Red << "Error: No input file provided!\n" << Color::Reset << parser.help();
        return 1;
    }

    // Read file
    std::ifstream in(files[0]);
    if (!in) {
        std::cout << Color::Red << "Error: Cannot open file: " << files[0] << Color::Reset << "\n";
        return 1;
    }

    std::string source((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    AOL_Lexer lexer(source);
    auto tokens = lexer.tokenize();

    if (parser.has("--lexout")) {
        std::cout << Color::Bold << "=== Token Dump ===" << Color::Reset << "\n\n";
        for (auto& t : tokens) PrintToken(t);
        std::cout << Color::Green << "Lexing complete." << Color::Reset << "\n";
        return 0;
    }

    return 0;
}
