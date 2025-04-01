#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <cstdio>
#include <cstdlib>

// Project Includes
#ifndef BUILD_basicio
#define BUILD_basicio
#endif
#include <basicio.h>
#include <errorHandler.h>

basicio_dll const char* FileIo(const char* filePath, FileMode mode, long seek, const char* content) {
    static char result[ReadBuffer]; // Temporary buffer for reading

    std::fstream file;
    std::ios_base::openmode openMode;

    // Set default seek values based on the mode
    long defaultSeek = 0;
    if (mode == FileMode::AppendNormal || mode == FileMode::AppendBinary || mode == FileMode::ReadAppendNormal || mode == FileMode::ReadAppendBinary) {
        defaultSeek = std::numeric_limits<long>::max(); // Append default seek to the end of file
    }

    // Determine open mode based on the file operation
    switch (mode) {
        case FileMode::WriteNormal:
            openMode = std::ios::out | std::ios::trunc;
            if (seek < 0) seek = defaultSeek; // Use default seek if no seek value is passed
            break;

        case FileMode::AppendNormal:
            openMode = std::ios::out | std::ios::app;
            if (seek < 0) seek = defaultSeek;
            break;

        case FileMode::ReadNormal:
            openMode = std::ios::in;
            if (seek < 0) seek = 0;
            break;

        case FileMode::WriteBinary:
            openMode = std::ios::out | std::ios::binary | std::ios::trunc;
            if (seek < 0) seek = defaultSeek;
            break;

        case FileMode::AppendBinary:
            openMode = std::ios::out | std::ios::binary | std::ios::app;
            if (seek < 0) seek = defaultSeek;
            break;

        case FileMode::ReadBinary:
            openMode = std::ios::in | std::ios::binary;
            if (seek < 0) seek = 0;
            break;

        case FileMode::ReadWriteNormal:
            openMode = std::ios::in | std::ios::out | std::ios::trunc;
            if (seek < 0) seek = 0;
            break;

        case FileMode::ReadAppendNormal:
            openMode = std::ios::in | std::ios::out | std::ios::app;
            if (seek < 0) seek = 0;
            break;

        case FileMode::ReadWriteBinary:
            openMode = std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc;
            if (seek < 0) seek = 0;
            break;

        case FileMode::ReadAppendBinary:
            openMode = std::ios::in | std::ios::out | std::ios::binary | std::ios::app;
            if (seek < 0) seek = 0;
            break;

        default:
            return ERROR_BASICIO; // Invalid mode
    }

    // Open the file
    file.open(filePath, openMode);
    if (!file) {
        return ERROR_BASICIO_FILENOTFOUND; // Return error if file can't be opened
    }

    // Seek to the specified position
    if (seek >= 0) {
        file.seekg(seek, std::ios::beg); // For reading
        file.seekp(seek, std::ios::beg); // For writing
    }

    // Perform file operations based on the mode
    switch (mode) {
        case FileMode::WriteNormal:
        case FileMode::WriteBinary: {
            if (content != nullptr) {
                file << content; // Write content to file
            }
            break;
        }

        case FileMode::AppendNormal:
        case FileMode::AppendBinary: {
            if (content != nullptr) {
                file << content; // Append content to file
            }
            break;
        }

        case FileMode::ReadNormal:
        case FileMode::ReadBinary: {
            // Read entire file contents
            std::string fileContent;
            std::string line;

            while (std::getline(file, line)) {
                fileContent += line + "\n";  // Preserve newlines
            }

            if (fileContent.empty()) {
                return ERROR_BASICIO;  // Handle empty files
            }

            // Copy to static buffer
            strncpy(result, fileContent.c_str(), ReadBuffer - 1);
            result[ReadBuffer - 1] = '\0';  // Ensure null termination
            break;
        }

        case FileMode::ReadWriteNormal:
        case FileMode::ReadWriteBinary: {
            // Read existing content, then write the new content
            file.clear(); // Clear EOF flag
            file.seekp(0, std::ios::beg); // Seek back to the start for writing
            if (content != nullptr) {
                file << content; // Write content
            }
            break;
        }

        case FileMode::ReadAppendNormal:
        case FileMode::ReadAppendBinary: {
            // Read existing content, then append the new content
            file.clear(); // Clear EOF flag
            file.seekp(0, std::ios::end); // Seek to the end for appending
            if (content != nullptr) {
                file << content; // Append content
            }
            break;
        }

        default:
            return ERROR_BASICIO; // Invalid operation
    }

    // Close the file
    file.close();
    if (mode != FileMode::ReadWriteNormal && mode != FileMode::ReadAppendBinary && mode != FileMode::ReadAppendNormal && mode != FileMode::ReadWriteBinary && mode != FileMode::ReadBinary && mode != FileMode::ReadNormal) {
        return ""; // For non-reading operations, return an empty string
    } else {
        return result;
    }
}
