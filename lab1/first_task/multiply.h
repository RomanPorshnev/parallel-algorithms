#pragma once


using Matrix = std::vector<std::vector<int>>;


Matrix GetMatrix(std::string &filename){
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