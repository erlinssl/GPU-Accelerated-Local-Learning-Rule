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


double CubeArray::calc(std::vector<std::vector<double>> x, size_t outer) {
    double sum = 0;
    for (size_t inner = 0; inner < 5; inner++) {
        for (size_t index = 0; index < 5; index++) {
            double value = x[inner][index] - this->w[outer][inner][index];
            sum += std::pow(value, 2);
        }
    }
    return -sum;
}