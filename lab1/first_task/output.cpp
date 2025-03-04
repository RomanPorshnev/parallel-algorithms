#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "multiply.h"


std::ostream& operator << (std::ostream &os, const Matrix &matrix)
{
    for (int i = 0; i < matrix.size(); i++){
        for (int j = 0; j < matrix[0].size(); j++){
            os << matrix[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}

int main(int argc, char *argv[]){
    std::string filename(argv[1]);
    Matrix matrix = GetMatrix(filename);
    std::cout << "Результат умножения матрицы из matrix1.txt на матрицу из matrix2: \n" << std::endl;
    std::cout << matrix;
    return 0;
}