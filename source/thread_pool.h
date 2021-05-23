#pragma once

#include <thread>
#include <vector>

template <auto Callback>
class ThreadPool {
public:
    ThreadPool(size_t thread_count, size_t file_count, size_t file_size_log2)
        : m_is_running(false)
    {
        m_threads.reserve(thread_count);
        for (size_t i = 0; i < thread_count; i++) {
            m_threads.push_back(std::thread(&ThreadPool::worker_thread, this, i * file_count / thread_count, (i + 1) * file_count / thread_count, file_size_log2));
        }
    }

    void join() {
        m_is_running = true;

        for (std::thread& thread : m_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

private:
    void worker_thread(size_t file_range_begin, size_t file_range_end, size_t file_size_log2) {
        while (!m_is_running);

        for (size_t file_index = file_range_begin; file_index < file_range_end; file_index++) {
            Callback(file_index, file_size_log2);
        }
    }

    std::vector<std::thread> m_threads;
    std::atomic_bool m_is_running;
};
