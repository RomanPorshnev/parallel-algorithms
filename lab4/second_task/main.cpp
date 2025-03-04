#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <future>

using Matrix = std::vector<std::vector<int>>;
using Sizes = std::pair<std::pair<int, int>, std::pair<int, int>>;
const int TRIVIAL_MULTIPLICATION_BOUND = 64;

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

Matrix MultiplyOperation(const Matrix &matrix1, const Matrix &matrix2){
    Matrix matrix (matrix1.size(), std::vector<int>(matrix2[0].size(), 0));
    for (int i = 0; i < matrix1.size(); i++){
        for (int j = 0; j < matrix2[0].size(); j++){
            int x = 0;
            for (int k = 0; k < matrix2.size(); k++){
                x += matrix1[i][k] * matrix2[k][j];
            }
            matrix[i][j] = x;
        }
    }
    return matrix;
}

void TransferData(Matrix &M11, Matrix &M12, Matrix &M21, Matrix &M22, const Matrix &matrix){
        for (int i = 0; i < M11.size(); i++){
            for (int j = 0; j < M11.size(); j++){
                M11[i][j] = matrix[i][j];
                M12[i][j] = matrix[i][j + M11.size()];
                M21[i][j] = matrix[i + M11.size()][j];
                M22[i][j] = matrix[i + M11.size()][j + M11.size()];
            }
        }
}

void CollectBlocks(const Matrix &C11, const Matrix &C12, const Matrix &C21, const Matrix &C22, Matrix &matrix){
    for (int i = 0; i < C11.size(); i++){
        for (int j = 0; j < C11.size(); j++){
            matrix[i][j] = C11[i][j];
            matrix[i][j + C11.size()] = C12[i][j];
            matrix[i + C11.size()][j] = C21[i][j];
            matrix[i + C11.size()][j + C11.size()] = C22[i][j];
        }
    }
}

Matrix add(const Matrix &matrix1, const Matrix &matrix2){
    Matrix matrix (matrix1.size(), std::vector<int>(matrix1.size(), 0));
    for (int i = 0; i < matrix1.size(); i++){
        for (int j = 0; j < matrix1[0].size(); j++){
            matrix[i][j] = matrix1[i][j] + matrix2[i][j];
        }
    }
    return matrix;
}

Matrix sub(const Matrix &matrix1, const Matrix &matrix2){
    Matrix matrix (matrix1.size(), std::vector<int>(matrix1.size(), 0));
    for (int i = 0; i < matrix1.size(); i++){
        for (int j = 0; j < matrix1[0].size(); j++){
            matrix[i][j] = matrix1[i][j] - matrix2[i][j];
        }
    }
    return matrix;
}

Matrix ShtrassenAlgorithm(const Matrix &matrix1, const Matrix &matrix2, int n_threads){
    int n = matrix1.size();
    if (n <= TRIVIAL_MULTIPLICATION_BOUND){
        return MultiplyOperation(matrix1, matrix2);
    }
    int newSize = n / 2;
    Matrix A11 (newSize, std::vector<int>(newSize, 0));
    Matrix A12 (newSize, std::vector<int>(newSize, 0));
    Matrix A21 (newSize, std::vector<int>(newSize, 0));
    Matrix A22 (newSize, std::vector<int>(newSize, 0));
    Matrix B11 (newSize, std::vector<int>(newSize, 0));
    Matrix B12 (newSize, std::vector<int>(newSize, 0));
    Matrix B21 (newSize, std::vector<int>(newSize, 0));
    Matrix B22 (newSize, std::vector<int>(newSize, 0));
    TransferData(A11, A12, A21, A22, matrix1);
    TransferData(B11, B12, B21, B22, matrix2);
    Matrix P1 (newSize, std::vector<int>(newSize, 0));
    Matrix P2 (newSize, std::vector<int>(newSize, 0));
    Matrix P3 (newSize, std::vector<int>(newSize, 0));
    Matrix P4 (newSize, std::vector<int>(newSize, 0));
    Matrix P5 (newSize, std::vector<int>(newSize, 0));
    Matrix P6 (newSize, std::vector<int>(newSize, 0));
    Matrix P7 (newSize, std::vector<int>(newSize, 0));
    std::vector<std::thread> threads;
    int residual = n_threads % 7;
    if (n_threads > 1){
        std::vector<std::future<Matrix>> futures;
        futures.push_back(std::async(std::launch::async, [&]() {
            return ShtrassenAlgorithm(add(A11, A22), add(B11, B22), n_threads / 7);
        }));

        futures.push_back(std::async(std::launch::async, [&]() {
            return ShtrassenAlgorithm(sub(A12, A22), add(B21, B22), n_threads / 7);
        }));

        futures.push_back(std::async(std::launch::async, [&]() {
            return ShtrassenAlgorithm(sub(A21, A11), add(B11, B12), n_threads / 7);
        }));

        futures.push_back(std::async(std::launch::async, [&]() {
            return ShtrassenAlgorithm(add(A11, A12), B22, n_threads / 7);
        }));

        futures.push_back(std::async(std::launch::async, [&]() {
            return ShtrassenAlgorithm(add(A21, A22), B11, n_threads / 7);
        }));

        futures.push_back(std::async(std::launch::async, [&]() {
            return ShtrassenAlgorithm(A22, sub(B21, B11), n_threads / 7);
        }));

        futures.push_back(std::async(std::launch::async, [&]() {
            return ShtrassenAlgorithm(A11, sub(B12, B22), n_threads / 7);
        }));
        P1 = futures[0].get();
        P2 = futures[1].get();
        P3 = futures[2].get();
        P4 = futures[3].get();
        P5 = futures[4].get();
        P6 = futures[5].get();
        P7 = futures[6].get();
    }
    else{
        P1 = ShtrassenAlgorithm(add(A11, A22), add(B11, B22), n_threads);
        P2 = ShtrassenAlgorithm(sub(A12, A22), add(B21, B22), n_threads);
        P3 = ShtrassenAlgorithm(sub(A21, A11), add(B11, B12), n_threads);
        P4 = ShtrassenAlgorithm(add(A11, A12), B22, n_threads);
        P5 = ShtrassenAlgorithm(add(A21, A22), B11, n_threads);
        P6 = ShtrassenAlgorithm(A22, sub(B21, B11), n_threads);
        P7 = ShtrassenAlgorithm(A11, sub(B12, B22), n_threads);
    }
    Matrix C11 = add(add(P1, P2), sub(P6, P4));
    Matrix C12 = add(P4, P7);
    Matrix C21 = add(P6, P5);
    Matrix C22 = add(add(P1, P3), sub(P7, P5));
    Matrix result (n, std::vector<int>(n, 0));
    CollectBlocks(C11, C12, C21, C22, result);
    return result;
}

void MultiplyMatrices(const std::string &filename1, const std::string &filename2, const std::string &filename, int &n_threads){
    Matrix matrix1 = GetMatrix(filename1);
    Matrix matrix2 = GetMatrix(filename2);
    Matrix matrix = ShtrassenAlgorithm(matrix1, matrix2, n_threads);
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
            lowBorder = -10;
            highBorder = 10;
            break;
    }
}

void ExpandMatrix(Matrix &matrix, const int &expandSize){
    int n = expandSize - matrix.size();
    int m = expandSize - matrix[0].size();
    std::vector<int> zeroRow(matrix[0].size(), 0);
    for (int i = 0; i < n; i++){
        matrix.push_back(zeroRow);
    }
    std::vector<int> zeroElements(expandSize - matrix[0].size(), 0);
    for (int i = 0; i < matrix.size(); i++){
        matrix[i].insert(matrix[i].end(), zeroElements.begin(), zeroElements.end());
    }
}

Matrix FillMatrix(const std::string &filename, const int &n, const int &m, const int &lowBorder, const int &highBorder){
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

Sizes GenerateMatrices(const std::string &filename1, const std::string &filename2){
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
    int maxSize = std::max({n1, m1, n2, m2});
    double degree = std::log2(maxSize);
    int expandSize = maxSize;
    if (degree - (int)degree > 0.000001) {
        expandSize = std::pow(2, (int)degree + 1);
    } 
    Matrix matrix1 = FillMatrix(filename1, n1, m1, lowBorder1, highBorder1);
    ExpandMatrix(matrix1, expandSize);
    WriteMatrix(matrix1, filename1);
    Matrix matrix2 = FillMatrix(filename2, n2, m2, lowBorder2, highBorder2);
    ExpandMatrix(matrix2, expandSize);
    WriteMatrix(matrix2, filename2);
    std::pair<int, int> sizeMatrix1 (n1, m1);
    std::pair<int, int> sizeMatrix2 (n2, m2);
    Sizes sizeMatrices (sizeMatrix1, sizeMatrix2);
    return sizeMatrices;
}

void PrintMatrix(const std::string &filename, const Sizes &sizeMatrices){
    std::cout << "Результат умножения матрицы из matrix1.txt на матрицу из matrix2.txt после удаления выравнивания: \n" << std::endl;
    Matrix matrix = GetMatrix(filename);
    for (int i = 0; i < sizeMatrices.first.first; i++){
        for (int j = 0; j < sizeMatrices.second.second; j++){
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
}

int main(int argc, char *argv[]){
    int n_threads = atoi(argv[4]);
    std::string filename1(argv[1]), filename2(argv[2]), filename(argv[3]);
    //Sizes sizeMatrices = GenerateMatrices(filename1, filename2);
    auto startTime = std::chrono::steady_clock::now();
    MultiplyMatrices(filename1, filename2, filename, n_threads);
    auto endTime = std::chrono::steady_clock::now();
    auto elapsedNS = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    //PrintMatrix(filename, sizeMatrices);
    std::cout << "\nВремя выполнения, мс: " << elapsedNS.count() << std::endl;
    return 0;
}