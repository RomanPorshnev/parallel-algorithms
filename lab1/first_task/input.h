#pragma once

using Matrix = std::vector<std::vector<int>>;

void WriteMatrix(Matrix &matrix, std::string &filename){
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