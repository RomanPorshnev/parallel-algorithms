#include <CL/cl2.hpp>
#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <chrono>

#define BLOCK_SIZE 32

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

void ExpandMatrix(Matrix &matrix){
    if (matrix.size() % BLOCK_SIZE){
        std::vector<int> zeroRow(matrix[0].size(), 0);
        int n = BLOCK_SIZE - (matrix.size() % BLOCK_SIZE);
        for (int i = 0; i < n; i++){
            matrix.push_back(zeroRow);
        }
    }
    if (matrix[0].size() % BLOCK_SIZE){
        std::vector<int> zeroElements(BLOCK_SIZE - (matrix[0].size() % BLOCK_SIZE), 0);
        for (int i = 0; i < matrix.size(); i++){
            matrix[i].insert(matrix[i].end(), zeroElements.begin(), zeroElements.end());
        }
    }
}

void FillMatrix(const std::string &filename, const int &n, const int &m, const int &lowBorder, const int &highBorder){
    Matrix matrix;
    for (int i = 0; i < n; i++){
        std::vector<int> row;
        for (int j = 0; j < m; j++){
            row.push_back(rand() % (highBorder - lowBorder) + lowBorder);
        }
        matrix.push_back(row);
    }
    ExpandMatrix(matrix);
    WriteMatrix(matrix, filename);
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
    FillMatrix(filename1, n1, m1, lowBorder1, highBorder1);
    FillMatrix(filename2, n2, m2, lowBorder2, highBorder2);
    std::pair<int, int> sizeMatrix1 (n1, m1);
    std::pair<int, int> sizeMatrix2 (n2, m2);
    Sizes sizeMatrices (sizeMatrix1, sizeMatrix2);
    return sizeMatrices;
}

void PrintMatrix(const std::string &filename, const Sizes &sizeMatrices){
    int numberOfZeroRows = BLOCK_SIZE - (sizeMatrices.first.first % BLOCK_SIZE);
    int numberOfZeroColumns = BLOCK_SIZE - (sizeMatrices.second.second % BLOCK_SIZE);
    std::cout << "Результат умножения матрицы из matrix1.txt на матрицу из matrix2.txt после удаления выравнивания: \n" << std::endl;
    Matrix matrix = GetMatrix(filename);
    for (int i = 0; i < matrix.size() - numberOfZeroRows; i++){
        for (int j = 0; j < matrix[0].size() - numberOfZeroColumns; j++){
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
}

std::vector<int> ConvertTo1DArray(const Matrix &matrix){
    std::vector<int> arr(matrix.size() * matrix[0].size());
    for (int i = 0; i < matrix.size(); i++){
        for (int j = 0; j < matrix[0].size(); j++){
            arr[i * matrix[0].size() + j] = matrix[i][j];
        }
    }
    return arr;
}

Matrix ConvertTo2DArray(const std::vector<int> &arr, const int &n, const int &m){
    Matrix matrix (n, std::vector<int>(m, 0));
    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++){
            matrix[i][j] = arr[i * m + j];
        }
    }
    return matrix;
}

void PrintArray(const std::vector<int> &vec){
    for (const auto &v: vec){
        std::cout << v << " ";
    }
    std::cout << "\n";
}

int main(int argc, char *argv[]){
    std::string filename1(argv[1]), filename2(argv[2]), filename(argv[3]);
    //Sizes sizeMatrices = GenerateMatrices(filename1, filename2);
    int n1 = GetMatrix(filename1).size();
    int m1 = GetMatrix(filename1)[0].size();
    int n2 = GetMatrix(filename2).size();
    int m2 = GetMatrix(filename2)[0].size();
    std::pair<int, int> sizeMatrix1 (n1, m1);
    std::pair<int, int> sizeMatrix2 (n2, m2);
    Sizes sizeMatrices (sizeMatrix1, sizeMatrix2);

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms.front();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    cl::Device device = devices.front();

    cl::Context context(device);
    cl::Program::Sources sources;

    std::ifstream multiplyFile("../src/multiply.cl");
    std::string kernelCode(std::istreambuf_iterator<char>(multiplyFile), (std::istreambuf_iterator<char>()));
    sources.emplace_back(kernelCode.c_str(), kernelCode.length() + 1);
    cl::Program program(context, sources);
    cl_int err = program.build(devices);
    if (err != CL_SUCCESS) {
        std::cerr << "Error building program." << std::endl;
        for (const auto& device : devices) {
            std::string buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
            std::cerr << "Build log for device " << device.getInfo<CL_DEVICE_NAME>() << ":" << std::endl;
            std::cerr << buildLog << std::endl;
        }

        return 1;
    }
    std::vector<int> matrix1 = ConvertTo1DArray(GetMatrix(filename1));
    std::vector<int> matrix2 = ConvertTo1DArray(GetMatrix(filename2));
    std::vector<int> matrix (sizeMatrices.first.first * sizeMatrices.second.second);

    std::cout << "Start computing...." << std::endl;
    auto startTime = std::chrono::steady_clock::now();
    cl::Buffer bufferMatrix1(context, CL_MEM_READ_ONLY, sizeof(int) * matrix1.size());
    cl::Buffer bufferMatrix2(context, CL_MEM_READ_ONLY, sizeof(int) * matrix2.size());
    cl::Buffer bufferMatrix(context, CL_MEM_WRITE_ONLY, sizeof(int) * matrix.size());
    
    cl::CommandQueue queue(context, device);
    queue.enqueueWriteBuffer(bufferMatrix1, CL_TRUE, 0, sizeof(int) * matrix1.size(), matrix1.data());
    queue.enqueueWriteBuffer(bufferMatrix2, CL_TRUE, 0, sizeof(int) * matrix2.size(), matrix2.data());
    cl::Kernel kernel(program, "block_multiply");

    kernel.setArg(0, bufferMatrix1);
    kernel.setArg(1, bufferMatrix2);
    kernel.setArg(2, bufferMatrix);
    kernel.setArg(3, sizeMatrices.first.first);
    kernel.setArg(4, sizeMatrices.first.second);
    kernel.setArg(5, sizeMatrices.second.second);

    cl::NDRange global(sizeMatrices.second.second, sizeMatrices.first.first);
    cl::NDRange local(BLOCK_SIZE, BLOCK_SIZE);

    queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
    queue.enqueueReadBuffer(bufferMatrix, CL_TRUE, 0, sizeof(int) * matrix.size(), matrix.data());

    auto endTime = std::chrono::steady_clock::now();
    auto elapsedNS = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    WriteMatrix(ConvertTo2DArray(matrix, sizeMatrices.first.first, sizeMatrices.second.second), filename);
    std::cout << "\nВремя выполнения, нс: " << elapsedNS.count() << std::endl;
    return 0;
}