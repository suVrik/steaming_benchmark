#pragma once

#include <Windows.h>

#include <cassert>

class FileReader {
public:
    FileReader()
        : file(INVALID_HANDLE_VALUE)
    {
    }

    FileReader(const char* path)
#ifdef USE_ASYNCHRONOUS_IO
        : file(CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL))
#else
        : file(CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
#endif // USE_ASYNCHRONOUS_IO
    {
        assert(file != INVALID_HANDLE_VALUE);
    }

    FileReader(FileReader&& other)
        : file(other.file)
    {
        other.file = INVALID_HANDLE_VALUE;
    }

    ~FileReader() {
        if (file != INVALID_HANDLE_VALUE) {
            BOOL result = CloseHandle(file);
            assert(result == TRUE);
        }
    }

    FileReader& operator=(FileReader&& other) {
        if (file != INVALID_HANDLE_VALUE) {
            BOOL result = CloseHandle(file);
            assert(result == TRUE);
        }

        file = other.file;
        other.file = INVALID_HANDLE_VALUE;

        return *this;
    }

#ifdef USE_ASYNCHRONOUS_IO
    void read(void* data, size_t size, OVERLAPPED& overlapped) {
#else
    void read(void* data, size_t size) {
#endif // USE_ASYNCHRONOUS_IO
        assert(file != INVALID_HANDLE_VALUE);

        DWORD read_;

#ifdef USE_ASYNCHRONOUS_IO
        BOOL result = ReadFile(file, data, static_cast<DWORD>(size), &read_, &overlapped);
#else
        BOOL result = ReadFile(file, data, static_cast<DWORD>(size), &read_, NULL);
#endif // USE_ASYNCHRONOUS_IO

        assert(result == TRUE);
        assert(static_cast<DWORD>(size) == read_);
    }

    HANDLE file;
};
