//
// Created by ingebrigt on 21.01.2022.
//

#ifndef FILTER_FINDER_MODEL_H
#define FILTER_FINDER_MODEL_H


class Model {
public:
    double sigma;
    double lambda;
    Model(double sigma_, double lambda_) : sigma(sigma_), lambda(lambda_) {}
    void update(int x);

private:
    double f(int i, int x);
};


#endif //FILTER_FINDER_MODEL_H
