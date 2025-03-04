#include "lock_free_queue.h"

lock_free_queue::lock_free_queue(){
    Node *node = new Node;
    head = node;
    tail = node;
    startHead = head.load();
}

lock_free_queue::~lock_free_queue(){
    Node *current = this->startHead.load();
    while (current){  
        Node *next = current->next.load();
        delete current;
        current = next;
    }
}

lock_free_queue& lock_free_queue::operator=(const lock_free_queue &other){
    if (&other != this){
        this->head.store(other.head.load());
        this->tail.store(other.tail.load());
    }
    return *this;
}

void lock_free_queue::enqueue(matrices someMatrices){
    Node *node = new Node(someMatrices);
    while (true){
        Node *last = this->tail.load();
        Node *next = last->next.load();
        if (last == this->tail.load()){ // для согласованности
            if (next == nullptr){
                Node *expected = nullptr;
                if (last->next.compare_exchange_weak(expected, node)){ // добавление нового узла в список
                    this->tail.compare_exchange_weak(last, node); // обновление хвоста (новый узел теперь будет хвостом)
                    return;
                }
            }
            else {
                this->tail.compare_exchange_weak(last, next); // фикс хвоста другого потока, который не успел его зафиксить самостоятельно
            }
        }
    }
}

void lock_free_queue::dequeue(matrices &value){
    Node *first = this->head.load();
    Node *last = this->tail.load();
    Node *next = first->next.load();
    if (first == last){
        if (next != nullptr){
            this->tail.compare_exchange_weak(last, next); // фикс хвоста другого потока, который добавил новый узел, но не успел его пофиксить самостоятельно
        }
    }
    else{
        matrices someMatrices = next->someMatrices;
        if (this->head.compare_exchange_weak(first, next)){ // фикс головы после извлечения матриц
            value = someMatrices;
        }
    }
}