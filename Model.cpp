//
// Created by ingebrigt on 21.01.2022.
//

#include "Model.h"

#include <utility>


double Model::f(int i, std::vector<std::vector<double>> x) {
    return std::exp(this->w.calc(std::move(x), i)/this->sigma);
}

std::vector<std::vector<double>> operator-(std::vector<std::vector<double>> x, std::vector<std::vector<double>> y) {
    for (int i = 0; i < x.size(); ++i) {
        for (int j = 0; j < x[i].size(); ++j) {
            x[i][j] = x[i][j] - y[i][j];
        }
    }
    return x;
}

void Model::update(std::vector<std::vector<double>> x) {
    auto diff = CubeArray(true, 16, 5, 5);
    // todo make 16 dynamic according to outermost shape
    for (int i1 = 0; i1 < 16; ++i1) {
        diff.w[i1] += f(i1, x) * (x - w.w[i1]);
    }
}
