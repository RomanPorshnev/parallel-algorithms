#pragma once

#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>


using Matrix = std::vector<std::vector<int>>;

struct matrices{
    Matrix leftMatrix;
    Matrix rightMatrix;
    Matrix resultMatrix;
};

class threadsafe_queue{
    private:
        mutable std::mutex writeMut;
        mutable std::mutex readMut;
        std::queue<matrices> dataQueue;
        std::condition_variable pushCond;
        std::condition_variable popCond;
        int maxSize;
    public:
        threadsafe_queue();
        threadsafe_queue(size_t);
        threadsafe_queue& operator=(const threadsafe_queue&) = delete;
        void push(matrices);
        void wait_and_pop(matrices &);
        bool empty() const;
        int size() const;
};