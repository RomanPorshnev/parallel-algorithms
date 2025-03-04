#pragma once
#include <iostream>
#include <atomic>
#include <vector>
#include <mutex>

using Matrix = std::vector<std::vector<int>>;

struct matrices{
    Matrix leftMatrix;
    Matrix rightMatrix;
    Matrix resultMatrix;
};

struct Node{
    Node(matrices someMatrices) : someMatrices(someMatrices), next(nullptr) {}
    Node() : next(nullptr) {}
    struct matrices someMatrices;
    std::atomic<Node*> next;
};

class lock_free_queue{
    private:
        std::atomic<Node*> head;
        std::atomic<Node*> tail;
        std::atomic<Node*> startHead;
        int count = 0;
    public:
        void clear();
        lock_free_queue();
        lock_free_queue& operator=(const lock_free_queue&);
        ~lock_free_queue();
        void enqueue(matrices);
        void dequeue(matrices &);
};