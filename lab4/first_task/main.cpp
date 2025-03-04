#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <chrono>


using Matrix = std::vector<std::vector<int>>;
using Sizes = std::pair<std::pair<int, int>, std::pair<int, int>>;
Matrix GetMatrix(std::string filename){
    std::ifstream in(filename);
    Matrix matrix;
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::vector<int> row;
        int num;
        while (iss >> num) {
            row.push_back(num);
        }
        matrix.push_back(row);
    }
    return matrix;
}

void WriteMatrix(const Matrix &matrix, const std::string &filename){
    std::ofstream out;
    out.open(filename);
    for (int i = 0; i < matrix.size(); i++){
        for (int j = 0; j < matrix[0].size() - 1; j++){
            out << matrix[i][j] << " ";
        }
        out << matrix[i][matrix[0].size() - 1];
        out << "\n";
    }
    out.close();
}

void MultiplyOperation(const Matrix &matrix1, const Matrix &matrix2, Matrix &matrix, 
    const int &rowMatrix1, const int &columnMatrix1, const int &rowMatrix2, const int &columnMatrix2, const int &rowMatrix, const int &columnMatrix, const int &blockSize){
    for (int i = 0; i < blockSize; i++){
        for (int j = 0; j < blockSize; j++){
            int x = 0;
            for (int k = 0; k < blockSize; k++){
                x += matrix1[i + rowMatrix1][k + columnMatrix1] * matrix2[k + rowMatrix2][j + columnMatrix2];
            }
            matrix[i + rowMatrix][j + columnMatrix] += x;
        }
    }
}

void MultiplyBlocks(const Matrix &matrix1, const Matrix &matrix2, Matrix &matrix, 
    const int &startBlock, const int &endBlock, const int &blocksPerRowInMatrix1, const int &blocksPerRowInMatrix, const int &blockSize){
    int blockRow, blockColumn;
    for (int currentBlock = startBlock; currentBlock < endBlock; currentBlock++){
        blockRow = currentBlock / blocksPerRowInMatrix;
        blockColumn = currentBlock % blocksPerRowInMatrix;
        for (int k = 0; k < blocksPerRowInMatrix1; k++){
            MultiplyOperation(matrix1, matrix2, matrix, blockRow * blockSize, k * blockSize, k * blockSize, blockColumn * blockSize, blockRow * blockSize, blockColumn * blockSize, blockSize);
        }
    }
}

void MultiplyMatrices(const std::string &filename1, const std::string &filename2, const std::string &filename, int &n_threads, const int &blockSize){
    Matrix matrix1 = GetMatrix(filename1);
    Matrix matrix2 = GetMatrix(filename2);
    Matrix matrix (matrix1.size(), std::vector<int>(matrix2[0].size(), 0));
    std::vector<std::thread> threads;
    int startBlock = 0, endBlock = 0;
    int resultNumberOfBlocks = (matrix1.size() / blockSize) * (matrix2[0].size() / blockSize);
    int blocksPerThread = resultNumberOfBlocks / n_threads;
    int residualBlocks = resultNumberOfBlocks % n_threads;
    int blocksPerRowInMatrix1 = matrix1[0].size() / blockSize;
    int blocksPerRowInMatrix = matrix[0].size() / blockSize;
    n_threads = std::min(n_threads, resultNumberOfBlocks);
    for (int i = 0; i < n_threads; i++){
        startBlock = endBlock;
        endBlock = startBlock + blocksPerThread + (i < residualBlocks ? 1 : 0);
        threads.emplace_back(MultiplyBlocks, std::ref(matrix1), std::ref(matrix2), std::ref(matrix), startBlock, endBlock, blocksPerRowInMatrix1, blocksPerRowInMatrix, blockSize);
    }
    for(auto &t: threads){
        t.join();
    }
    WriteMatrix(matrix, filename);
}

void Dialog(std::string nameMatrix, int &n, int &m, int &lowBorder, int &highBorder){
    char enterRange;
    std::cout << "Введите количество строк " << nameMatrix << " матрицы: ";
    std::cin >> n;
    std::cout << "Введите количество столбцов " << nameMatrix << " матрицы: ";
    std::cin >> m;
    std::cout << "Если хотите ввести диапазон генерации чисел матрицы введите [1], а иначе -- не [1]: ";
    std::cin >> enterRange;
    switch (enterRange){
        case '1':
            std::cout << "Введите нижнюю границу: ";
            std::cin >> lowBorder;
            std::cout << "Введите верхнюю границу: ";
            std::cin >> highBorder;
            break;
        default:
            lowBorder = -1;
            highBorder = 1;
            break;
    }
}

void ExpandMatrix(Matrix &matrix, const int &blockSize){
    if (matrix.size() % blockSize){
        std::vector<int> zeroRow(matrix[0].size(), 0);
        int n = blockSize - (matrix.size() % blockSize);
        for (int i = 0; i < n; i++){
            matrix.push_back(zeroRow);
        }
    }
    if (matrix[0].size() % blockSize){
        std::vector<int> zeroElements(blockSize - (matrix[0].size() % blockSize), 0);
        for (int i = 0; i < matrix.size(); i++){
            matrix[i].insert(matrix[i].end(), zeroElements.begin(), zeroElements.end());
        }
    }
}

void FillMatrix(const std::string &filename, const int &n, const int &m, const int &lowBorder, const int &highBorder, const int &blockSize){
    Matrix matrix;
    for (int i = 0; i < n; i++){
        std::vector<int> row;
        for (int j = 0; j < m; j++){
            row.push_back(rand() % (highBorder - lowBorder) + lowBorder);
        }
        matrix.push_back(row);
    }
    ExpandMatrix(matrix, blockSize);
    WriteMatrix(matrix, filename);
}

Sizes GenerateMatrices(const std::string &filename1, const std::string &filename2, const int &blockSize){
    int n1, m1 = 0, n2 = 1, m2, lowBorder1, highBorder1, lowBorder2, highBorder2;
    std::string nameMatrix1("первой"), nameMatrix2("второй");
    while (true){
        Dialog(nameMatrix1, n1, m1, lowBorder1, highBorder1);
        std::cout << "\n";
        Dialog(nameMatrix2, n2, m2, lowBorder2, highBorder2);
        std::cout << "\n";
        if (m1 != n2){
            std::cout << "Количество столбцов первой матрицы должно быть равно количеству строк второй матрицы" << std::endl;
        }
        else{
            break;
        }
    }
    FillMatrix(filename1, n1, m1, lowBorder1, highBorder1, blockSize);
    FillMatrix(filename2, n2, m2, lowBorder2, highBorder2, blockSize);
    std::pair<int, int> sizeMatrix1 (n1, m1);
    std::pair<int, int> sizeMatrix2 (n2, m2);
    Sizes sizeMatrices (sizeMatrix1, sizeMatrix2);
    return sizeMatrices;
}

void PrintMatrix(const std::string &filename, const Sizes &sizeMatrices, const int &blockSize){
    int numberOfZeroRows = blockSize - (sizeMatrices.first.first % blockSize);
    int numberOfZeroColumns = blockSize - (sizeMatrices.second.second % blockSize);
    std::cout << "Результат умножения матрицы из matrix1.txt на матрицу из matrix2.txt после удаления выравнивания: \n" << std::endl;
    Matrix matrix = GetMatrix(filename);
    for (int i = 0; i < matrix.size() - numberOfZeroRows; i++){
        for (int j = 0; j < matrix[0].size() - numberOfZeroColumns; j++){
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
}

int main(int argc, char *argv[]){
    int blockSize = atoi(argv[5]);
    int n_threads = atoi(argv[4]);
    std::string filename1(argv[1]), filename2(argv[2]), filename(argv[3]);
    //Sizes sizeMatrices = GenerateMatrices(filename1, filename2, blockSize);
    auto startTime = std::chrono::steady_clock::now();
    MultiplyMatrices(filename1, filename2, filename, n_threads, blockSize);
    auto endTime = std::chrono::steady_clock::now();
    auto elapsedNS = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    //PrintMatrix(filename, sizeMatrices, blockSize);
    std::cout << "\nВремя выполнения, нс: " << elapsedNS.count() << std::endl;
    return 0;
}