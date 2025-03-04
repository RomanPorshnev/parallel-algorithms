#include "threadsafe_queue.h"
#include <iostream>

threadsafe_queue::threadsafe_queue(size_t maxSize) : maxSize(maxSize) {}

threadsafe_queue::threadsafe_queue() : maxSize(10) {}

void threadsafe_queue::push(matrices someMatrices){
    std::unique_lock<std::mutex> ul(this->writeMut);
    popCond.wait(ul, [this]{return dataQueue.size() <= this->maxSize;});
    dataQueue.push(someMatrices);
    pushCond.notify_one();
}

void threadsafe_queue::wait_and_pop(matrices &value){
    std::unique_lock<std::mutex> ul(this->readMut);
    pushCond.wait(ul, [this]{return !dataQueue.empty();});
    value = dataQueue.front();
    dataQueue.pop();
    popCond.notify_one();
}

bool threadsafe_queue::empty() const{
    std::lock_guard<std::mutex> lk(this->readMut);
    return dataQueue.empty();
}

int threadsafe_queue::size() const{
    std::lock_guard<std::mutex> lk(this->readMut);
    return dataQueue.size();
}