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

void WriteMatrix(Matrix matrix, std::string filename){
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

void MultiplyMatrices(std::string filename1, std::string filename2, std::string filename, int n_threads){
    Matrix matrix1 = GetMatrix(filename1);
    Matrix matrix2 = GetMatrix(filename2);
    Matrix matrix (matrix1.size(), std::vector<int>(matrix2[0].size(), 0));
    std::vector<std::thread> threads;
    int border = std::min(n_threads, static_cast<int>(matrix1.size()));
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


void FillMatrix(std::string &filename, int n, int m, int lowBorder, int highBorder){
    Matrix matrix;
    for (int i = 0; i < n; i++){
        std::vector<int> row;
        for (int j = 0; j < m; j++){
            row.push_back(rand() % (highBorder - lowBorder) + lowBorder);
        }
        matrix.push_back(row);
    }
    WriteMatrix(matrix, filename);
}

void GenerateMatrices(std::string filename1, std::string filename2){
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
    FillMatrix(filename1, n1, m1, lowBorder1, highBorder1);
    FillMatrix(filename2, n2, m2, lowBorder2, highBorder2);
}

void PrintMatrix(std::string filename){
    std::cout << "Результат умножения матрицы из matrix1.txt на матрицу из matrix2.txt: \n" << std::endl;
    Matrix matrix = GetMatrix(filename);
    for (int i = 0; i < matrix.size(); i++){
        for (int j = 0; j < matrix[0].size(); j++){
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
}

int main(int argc, char *argv[]){
    int n_threads = atoi(argv[4]);
    std::string filename1(argv[1]), filename2(argv[2]), filename(argv[3]);
    std::thread input(GenerateMatrices, filename1, filename2);
    input.join();
    auto startTime = std::chrono::steady_clock::now();
    std::thread multiply(MultiplyMatrices, filename1, filename2, filename, n_threads);
    multiply.join();
    auto endTime = std::chrono::steady_clock::now();
    auto elapsedNS = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    std::thread output(PrintMatrix, filename);
    output.join();
    std::cout << "\nВремя выполнения, нс: " << elapsedNS.count() << std::endl;
    return 0;
}