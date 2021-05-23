#pragma once

#include <Windows.h>

#include <cassert>

class FileWriter {
public:
    FileWriter(const char* path)
        : file(CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))
    {
        assert(file != INVALID_HANDLE_VALUE);
    }

    FileWriter(FileWriter&& other)
        : file(other.file)
    {
        other.file = INVALID_HANDLE_VALUE;
    }

    ~FileWriter() {
        if (file != INVALID_HANDLE_VALUE) {
            BOOL result = CloseHandle(file);
            assert(result == TRUE);
        }
    }

    FileWriter& operator=(FileWriter&& other) {
        if (file != INVALID_HANDLE_VALUE) {
            BOOL result = CloseHandle(file);
            assert(result == TRUE);
        }

        file = other.file;
        other.file = INVALID_HANDLE_VALUE;

        return *this;
    }

    void write(void* data, size_t size) {
        assert(file != INVALID_HANDLE_VALUE);

        DWORD written;
        BOOL result = WriteFile(file, data, static_cast<DWORD>(size), &written, NULL);

        assert(result == TRUE);
        assert(static_cast<DWORD>(size) == written);
    }

    HANDLE file;
};
