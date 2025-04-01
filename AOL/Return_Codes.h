#ifndef RETURN_CODES_H
#define RETURN_CODES_H

#include <cstdint>

namespace ReturnCodes {

    enum class General : std::uint8_t {
        Success = 0,
        Error = 1,
        Warning = 2,
        Timeout = 3,
        OutOfMemory = 4,
        Unknown = 255
    };

    enum class FileSystem : std::uint8_t {
        Success = 0,
        FileNotFound = 10,
        PermissionDenied = 11,
        DiskFull = 12,
        InvalidPath = 13,
        Unknown = 255
    };

    enum class Parser : std::uint8_t {
        Success = 0,
        SyntaxError = 20,
        SemanticError = 21,
        UnexpectedToken = 22,
        MissingToken = 23,
        Unknown = 255
    };

    enum class Lexer : std::uint8_t {
        Success = 0,
        InvalidCharacter = 30,
        UnterminatedString = 31,
        NumberFormatError = 32,
        TokenTooLong = 33,
        Unknown = 255
    };

    enum class Argument : std::uint8_t {
        Success = 0,
        MissingArgument = 40,
        InvalidArgument = 41,
        TooManyArguments = 42,
        ConflictingArguments = 43,
        Unknown = 255
    };
} // namespace ReturnCodes

#endif // RETURN_CODES_H