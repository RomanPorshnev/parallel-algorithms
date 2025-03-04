#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <chrono>

int main(int argc, char *argv[]){
    pid_t pid1 = fork();
    if (pid1 == 0){
        execlp("./input", "input", argv[1], argv[2], argv[4], nullptr);
        perror("execlp");
        return 1;
    }
    waitpid(pid1, nullptr, 0);
    auto startTime = std::chrono::steady_clock::now();
    pid_t pid2 = fork();
    if (pid2 == 0){
        execlp("./multiply", "multiply", argv[1], argv[2], argv[3], nullptr);
        perror("execlp");
        return 1;
    }
    waitpid(pid2, nullptr, 0);
    auto endTime = std::chrono::steady_clock::now();
    auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    pid_t pid3 = fork();
    if (pid3 == 0){
        execlp("./output", "output", argv[3], nullptr);
        perror("execlp");
        return 1;
    }
    waitpid(pid3, nullptr, 0);
    std::cout << "\nВремя выполнения, нс: " << elapsed_ns.count() << std::endl;
    return 0;
}