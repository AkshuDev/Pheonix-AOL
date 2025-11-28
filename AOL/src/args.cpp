#include <args.hpp>
#include <iostream>

ArgParser::ArgParser(std::string programName) : program(std::move(programName)) {}

void ArgParser::addOption(const std::string& shortName, const std::string& longName, const std::string& description, bool requiresValue) {
    options.push_back({shortName, longName, description, requiresValue, false, std::nullopt});
}

ArgParser::Option* ArgParser::find(const std::string& name) {
    for (auto& opt : options) {
        if (opt.shortName == name || opt.longName == name)
            return &opt;
    }
    return nullptr;
}

bool ArgParser::parse(int argc, char** argv, bool& showHelp) {
    showHelp = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            showHelp = true;
            return true;
        }

        if (arg.starts_with("-")) {
            auto opt = find(arg);
            if (!opt) {
                std::cerr << Color::Red << "Error: Unknown option '" << arg << "'" << Color::Reset << "\n";
                return false;
            }

            opt->found = true;

            if (opt->requiresValue) {
                if (i + 1 >= argc) {
                    std::cerr << Color::Red << "Error: Missing value for '" << arg << "'" << Color::Reset << "\n";
                    return false;
                }
                opt->value = argv[++i];
            }
        } else {
            positionalArgs.push_back(arg);
        }
    }

    return true;
}

bool ArgParser::has(const std::string& name) const {
    for (const auto& opt : options)
        if (opt.shortName == name || opt.longName == name)
            return opt.found;
    return false;
}

std::optional<std::string> ArgParser::get(const std::string& name) const {
    for (const auto& opt : options)
        if ((opt.shortName == name || opt.longName == name) && opt.value.has_value())
            return opt.value;
    return std::nullopt;
}

std::vector<std::string> ArgParser::positional() const {
    return positionalArgs;
}

std::string ArgParser::help() const {
    std::string h = Color::Cyan + "Usage:" + Color::Reset + "\n";
    h += "  " + program + " [options] <input files>\n\n";

    h += Color::Cyan + "Options:" + Color::Reset + "\n";
    for (const auto& opt : options) {
        h += "  " + Color::Green + opt.shortName + ", " + opt.longName + Color::Reset;
        if (opt.requiresValue)
            h += " <value>";
        h += "\n\t" + opt.description + "\n";
    }
    return h;
}
