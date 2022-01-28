//
// Created by ingebrigt on 27.01.2022.
//

#include "Arrays.h"

CubeArray::CubeArray(bool zero, int outer, int middle, int inner) {
    for (int i = 0; i < outer; ++i) {
        w.emplace_back(std::vector<std::vector<double>>());
        for (int j = 0; j < middle; ++j) {
            w[i].emplace_back(std::vector<double>());
            for (int k = 0; k < inner; ++k) {
                if (zero) w[i][j].emplace_back(0);
                    // todo use c++ 11 instead
                else w[i][j].emplace_back(((double) rand() / RAND_MAX) * (1 - 0));
            }
        }
    }
}


double CubeArray::calc(SquareArray x, size_t outer) {
    double sum = 0;
    for (size_t inner = 0; inner < 5; inner++) {
        for (size_t index = 0; index < 5; index++) {
            double value = x[inner][index] - this->w[outer][inner][index];
            sum += std::pow(value, 2);
        }
    }
    return -sum;
}

SquareArray::SquareArray(std::vector<std::vector<double>> x) {
    arr = x;
}

size_t SquareArray::size() {
    return arr.size();
}

std::vector<double> SquareArray::operator[](size_t i) {
    return arr[i];
}

SquareArray operator*(double x, SquareArray y) {
    for (int i = 0; i < y.size(); i++) {
        for (auto &val : y[i]) {
            val *= x;
        }
    }
    return y;
}

SquareArray SquareArray::operator-(std::vector<std::vector<double>> y) {
    for (int i = 0; i < this->size(); ++i) {
        for (int j = 0; j < this[i].size(); ++j) {
            (*this)[i][j] = (*this)[i][j] - y[i][j];
        }
    }
    return *this;
}

SquareArray operator+=(std::vector<std::vector<double>> x, SquareArray y) {
    for (int i = 0; i < x.size(); ++i) {
        for (int j = 0; j < y.size(); ++j) {
            x[i][j] += y[i][j];
        }
    }
    return SquareArray(x);
}