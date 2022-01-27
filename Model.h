//
// Created by ingebrigt on 21.01.2022.
//

#ifndef FILTER_FINDER_MODEL_H
#define FILTER_FINDER_MODEL_H


#include <memory>
#include "Arrays.h"

class Model {
public:
    CubeArray w;
    double sigma;
    double lambda;
    explicit Model(double sigma_, double lambda_) : sigma(sigma_), lambda(lambda_), w(false, 16, 5, 5) {};
    void update(std::vector<std::vector<double>> x);

private:
    double f(int i, std::vector<std::vector<double>> x);
};


#endif //FILTER_FINDER_MODEL_H
