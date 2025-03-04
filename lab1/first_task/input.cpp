#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "input.h"

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


void FillMatrix(std::string &filename, int &n, int &m, int &lowBorder, int &highBorder){
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

void GenerateMatrices(std::string &filename1, std::string &filename2){
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

int main(int argc, char *argv[]){
    std::string filename1(argv[1]), filename2(argv[2]);
    int ignoreFlag = atoi(argv[3]);
    if (ignoreFlag){
        GenerateMatrices(filename1, filename2);
    }
    return 0;
}