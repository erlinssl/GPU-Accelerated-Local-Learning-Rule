//
// Created by ingebrigt on 21.01.2022.
//

#include "Model.h"

#include <utility>


double Model::f(int i, SquareArray x) {
    return std::exp(this->w.calc(std::move(x), i)/this->sigma);
}




void Model::update(SquareArray x) {
    auto diff = CubeArray(true, 16, 5, 5);
    // todo make 16 dynamic according to outermost shape
    for (int i1 = 0; i1 < 16; ++i1) {
        diff.w[i1] += f(i1, x) * (x - w.w[i1]);
    }
}
