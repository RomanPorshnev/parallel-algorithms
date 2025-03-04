#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "input.h"
#include "multiply.h"

void MultiplyMatrices(std::string &filename1, std::string &filename2, std::string &filename){
    Matrix matrix1 = GetMatrix(filename1);
    Matrix matrix2 = GetMatrix(filename2);
    Matrix matrix;
    for (int i = 0; i < matrix1.size(); i++){
        std::vector<int> row;
        for (int j = 0; j < matrix2[0].size(); j++){
            int x = 0;
            for (int k = 0; k < matrix2.size(); k++){
                x += matrix1[i][k] * matrix2[k][j];
            }
            row.push_back(x);
        }
        matrix.push_back(row);
    }
    WriteMatrix(matrix, filename);
}

int main(int argc, char *argv[]){
    std::string filename1(argv[1]), filename2(argv[2]), filename(argv[3]);
    MultiplyMatrices(filename1, filename2, filename);
    return 0;
}