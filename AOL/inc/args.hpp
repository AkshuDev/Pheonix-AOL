#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <colors.hpp>

class ArgParser {
public:
    struct Option {
        std::string shortName;
        std::string longName;
        std::string description;
        bool requiresValue = false;
        bool found = false;
        std::optional<std::string> value;
    };

    ArgParser(std::string programName);

    void addOption(const std::string& shortName, const std::string& longName, const std::string& description, bool requiresValue = false);

    bool parse(int argc, char** argv, bool& showHelp);

    bool has(const std::string& name) const;
    std::optional<std::string> get(const std::string& name) const;

    std::vector<std::string> positional() const;
    std::string help() const;

private:
    std::string program;
    std::vector<Option> options;
    std::vector<std::string> positionalArgs;

    Option* find(const std::string& name);
};
