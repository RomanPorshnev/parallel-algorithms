#include <iostream>
#include "lock_free_queue.h"
#include <thread>
#include <fstream>


std::mutex fileMut;
lock_free_queue lf_queue;


void WriteMatrix(Matrix &matrix){
    std::string filename("logs.txt");
    std::ofstream out;
    out.open(filename, std::ios::app);
    for (int i = 0; i < matrix.size(); i++){
        for (int j = 0; j < matrix[0].size() - 1; j++){
            out << matrix[i][j] << " ";
        }
        out << matrix[i][matrix[0].size() - 1];
        out << "\n";
    }
    out << "\n";
    out.close();
}

void WriteMatrices(matrices &someMatrices){
    std::lock_guard<std::mutex> lk(fileMut);
    WriteMatrix(someMatrices.leftMatrix);
    WriteMatrix(someMatrices.rightMatrix);
    WriteMatrix(someMatrices.resultMatrix);
}

void MultiplyOperation(Matrix &matrix1, Matrix &matrix2, Matrix &matrix, int fromRow, int toRow){
    for (int i = fromRow; i < toRow; i++){
        std::vector<int> row;
        for (int j = 0; j < matrix2[0].size(); j++){
            int x = 0;
            for (int k = 0; k < matrix2.size(); k++){
                x += matrix1[i][k] * matrix2[k][j];
            }
            matrix[i][j] = x;
        }
    }
}

void MultiplyMatrices(matrices &someMatrices){
    Matrix matrix1 = someMatrices.leftMatrix;
    Matrix matrix2 = someMatrices.rightMatrix;
    Matrix matrix (matrix1.size(), std::vector<int>(matrix2[0].size(), 0));
    std::vector<std::thread> threads;
    int border = std::min(4, static_cast<int>(matrix1.size()));
    int rows_per_thread = matrix1.size() / border;
    int mod = matrix1.size() % border;
    int fromRow = 0, toRow = 0;
    for (int i = 0; i < border; i++){
        fromRow = toRow;
        if (i < mod){
            toRow = fromRow + rows_per_thread + 1;
        }
        else{
            toRow = fromRow + rows_per_thread;
        }
        threads.emplace_back(MultiplyOperation, std::ref(matrix1), std::ref(matrix2), std::ref(matrix), fromRow, toRow);
    }

    for(auto &t: threads){
        t.join();
    }
    someMatrices.resultMatrix = matrix;
}

Matrix FillMatrix(int n, int m, int lowBorder, int highBorder){
    Matrix matrix;
    for (int i = 0; i < n; i++){
        std::vector<int> row;
        for (int j = 0; j < m; j++){
            row.push_back(rand() % (highBorder - lowBorder) + lowBorder);
        }
        matrix.push_back(row);
    }
    return matrix;
}

matrices GenerateMatrices(){
    int lowBorder1 = -10, highBorder1 = 10, lowBorder2 = -10, highBorder2 = 10;
    int n1 = rand() % 3 + 1;
    int m1 = rand() % 3 + 1;
    int n2 = m1;
    int m2 = rand() % 3 + 1;
    Matrix leftMatrix = FillMatrix(n1, m1, lowBorder1, highBorder1);
    Matrix rightMatrix = FillMatrix(n2, m2, lowBorder2, highBorder2);
    matrices someMatrices{leftMatrix, rightMatrix};
    return someMatrices;
}

void Producer(int id, int nTasks){
    for (int i = 0; i < nTasks; i++){
        matrices someMatrices = GenerateMatrices();
        lf_queue.enqueue(someMatrices);
        std::cout << "Producer " << id << " сгенерировал матрицы" << std::endl;
    }
}

void Consumer(int id){
    while (true){
        matrices someMatrices;
        lf_queue.dequeue(someMatrices);
        std::cout << "Consumer " << id << " получил матрицы" << std::endl;
        if (someMatrices.leftMatrix.size() == 0){
            std::cout << "Consumer " << id << " завершил работу" << std::endl;
            break;
        }
        MultiplyMatrices(someMatrices);
        WriteMatrices(someMatrices);
    }
}

void InputData(int &nProducers, int &nConsumers, int &nTasks){
    std::cout << "Введите количество производителей: ";
    std::cin >> nProducers;
    std::cout << "Введите количество потребителей: ";
    std::cin >> nConsumers;
    std::cout << "Введите количество пар матриц на одного производителя: ";
    std::cin >> nTasks;
}

int main(){
    int nProducers, nConsumers, nTasks;
    InputData(nProducers, nConsumers, nTasks);
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    for (int i = 0; i < nProducers; i++){
        producers.emplace_back(Producer, i + 1, nTasks);
    }
    for (int i = 0; i < nConsumers; i++){
        consumers.emplace_back(Consumer, i + 1);
    }
    for (auto& p : producers){
        p.join();
    }
    Matrix matrix;
    matrices someMatrices{matrix};
    for (int i = 0; i < nConsumers; i++){
        lf_queue.enqueue(someMatrices);
    }
    for (auto& c : consumers){
        c.join();
    }
    return 0;
}