#ifndef BASICIO_H
#define BASICIO_H

#define ERROR_BASICIO "$$ERROR$$BasicIO"
#define ERROR_BASICIO_FILENOTFOUND "$$ERROR$$BasicIO@FileNotFound@!"

#ifdef _WIN32
#ifdef BUILD_basicio
#define basicio_dll __declspec(dllexport)
#else
#define basicio_dll __declspec(dllimport)
#endif
#endif

#define ReadBuffer 1024 * 10

enum class FileMode {
    WriteNormal=1,      // 1. Write to file (Normal) [default seek = 0]
    AppendNormal,     // 2. Append to File (Normal) [default seek = end of file]
    ReadNormal,       // 3. Read File (Normal) [default seek = 0]
    WriteBinary,      // 4. Write to file (Binary) [default seek = 0]
    AppendBinary,     // 5. Append to file (Binary) [default seek = end of file]
    ReadBinary,       // 6. Read File (Binary) [default seek = 0]
    ReadWriteNormal,  // 7. Read and Write file (Normal) [default seek = 0]
    ReadAppendNormal,  // 8. Read and Append file (Normal) [default seek for reading = 0, for appending = end of file]
    ReadWriteBinary,  // 9. Read and Write file (Binary) [default seek = 0]
    ReadAppendBinary   // 10. Read and Append file (Binary) [default seek for reading = 0, for appending = end of file]
};

extern "C" {
    basicio_dll const char* FileIo(const char* filePath, FileMode mode, long seek, const char* content);
}

#endif