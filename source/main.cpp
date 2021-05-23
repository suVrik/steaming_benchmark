#include "file_reader.h"
#include "file_writer.h"
#include "thread_pool.h"

#include <chrono>
#include <cstdio>

constexpr size_t FILE_SIZE_MIN_LOG2 = 7;
constexpr size_t FILE_SIZE_MAX_LOG2 = 26;

constexpr size_t FILE_COUNT = 1024;
constexpr size_t THREAD_COUNT = 16;

static char file_names[FILE_COUNT][16];
static uint8_t file_data[1 << FILE_SIZE_MAX_LOG2];

static FileReader file_readers[FILE_COUNT];
static OVERLAPPED file_overlapped[FILE_COUNT];

void create_file_names() {
    for (size_t i = 0; i < FILE_COUNT; i++) {
        sprintf(file_names[i], "file%04zu.bin", i);
    }
}

void create_files() {
    {
        for (uint8_t& value : file_data) {
            value = static_cast<char>(rand() & 0xFF);
        }

        FileWriter file("file.bin");
        file.write(file_data, sizeof(file_data));
    }

    for (size_t i = 0; i < FILE_COUNT; i++) {
        BOOL result = CopyFile("file.bin", file_names[i], FALSE);
        assert(result == TRUE);
    }
}

void read_file(size_t file_index, size_t file_size_log2) {
#ifdef USE_ASYNCHRONOUS_IO
    file_readers[file_index] = FileReader(file_names[file_index]);
    file_readers[file_index].read(file_data, static_cast<size_t>(1) << file_size_log2, file_overlapped[file_index]);
#else
    FileReader file(file_names[file_index]);
    file.read(file_data, static_cast<size_t>(1) << file_size_log2);
#endif // USE_ASYNCHRONOUS_IO
}

int main(int argc, char* argv[]) {
    create_file_names();

    create_files();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    for (size_t file_size_log2 = FILE_SIZE_MIN_LOG2; file_size_log2 < FILE_SIZE_MAX_LOG2; file_size_log2++) {
#ifdef USE_MULTITHREADING
        ThreadPool<&read_file> thread_pool(THREAD_COUNT, FILE_COUNT, file_size_log2);

        auto t1 = std::chrono::high_resolution_clock::now();

        thread_pool.join();

        auto t2 = std::chrono::high_resolution_clock::now();
#else
        auto t1 = std::chrono::high_resolution_clock::now();

        for (size_t file_index = 0; file_index < FILE_COUNT; file_index++) {
            read_file(file_index, file_size_log2);
        }

        auto t2 = std::chrono::high_resolution_clock::now();
#endif // USE_MULTITHREADING

        printf("%f\n", std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() / 1000000.f / FILE_COUNT);

#ifdef USE_ASYNCHRONOUS_IO
        for (size_t file_index = 0; file_index < FILE_COUNT; file_index++) {
            DWORD number_of_bytes_transferred;
            BOOL result = GetOverlappedResult(file_readers[file_index].file, &file_overlapped[file_index], &number_of_bytes_transferred, TRUE);

            assert(result == TRUE);
            assert(static_cast<size_t>(number_of_bytes_transferred) == static_cast<size_t>(1) << file_size_log2);

            // We're not measuring file close.
            file_readers[file_index] = FileReader();
        }
#endif // USE_ASYNCHRONOUS_IO
    }

    return 0;
}
