//
// Created by ingebrigt on 21.01.2022.
//

#ifndef FILTER_FINDER_MODEL_H
#define FILTER_FINDER_MODEL_H


#include <memory>
#include <string>
#include "Arrays.h"
#include <filesystem>

template <typename T>
class Model {
public:
    CubeArray<T> w;
    double sigma;
    double lambda;
    explicit Model(double sigma_, double lambda_) : sigma(sigma_), lambda(lambda_), w(false, 16, 5, 5) {};
    explicit Model(const char subfigure) : sigma(1.0), lambda(0.5), w(false, 16, 5, 5) {this->load(subfigure);};
    void update(SquareArray<T> x);

    void save(const char &subfigure);
    bool load(const char &subfigure);

private:
    double f(int i, SquareArray<T> x);
};


#endif //FILTER_FINDER_MODEL_H
