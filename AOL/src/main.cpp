#include <iostream>
#include <fstream>
#include <memory>

#include <args.hpp>
#include <colors.hpp>
#include <lexer.hpp>
#include <parser.hpp>

#include <compiler_amd64.hpp>

int main(int argc, char** argv) {
    ArgParser parser(argv[0]);

    parser.addOption("-o", "--output", "Specify output file name", true, true);
    parser.addOption("-v", "--verbose", "Enable verbose mode", false, false);
    parser.addOption("-c", "--compile-only", "Compile but do not JIT", false, false);
    parser.addOption("", "--lexout", "Stop after lexing and print all tokens", false, false);
    parser.addOption("-a", "--arch", "Target Architecture, Default: amd64", true, false);
    parser.addOption("-b", "--bits", "Target Bits, Default: 64", true, false);

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

    AOL_Parser aol_parser(tokens);
    std::shared_ptr<ASTNode> astroot = aol_parser.parseProgram();

    auto archOpt = parser.get("-a");
    std::string arch;
    std::string out;
    if (archOpt.has_value()) {
        arch = archOpt.value();
    } else {
        arch = "amd64";
    }

    if (arch == "amd64") {
        Compiler_Amd64 compiler;
        out = compiler.compile(astroot);
    } else {
        std::cerr << Color::Red << "Error: Unsupported Architecture '" << arch << "'" << Color::Reset << "\n";
        return 1;
    }

    auto outOpt = parser.get("-o");
    std::string outPath = outOpt.value_or("a.pasm"); 

    std::ofstream outfile(outPath);
    if (outfile.is_open()) {
        outfile << out;
    } else {
        std::cerr << Color::Red << "Error: Could not open output file!" << Color::Reset << "\n";
    }

    outfile.close();

    return 0;
}
