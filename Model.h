//
// Created by ingebrigt on 21.01.2022.
//

#ifndef FILTER_FINDER_MODEL_H
#define FILTER_FINDER_MODEL_H


#include <memory>
#include <string>
#include <filesystem>

#include <arrayfire.h>

#include "Arrays.h"


template <typename T>
class Model {
public:
    CubeArray<T> w;
    double sigma;
    double lambda;
    int filters;
    int resolution;
    explicit Model(double sigma_, double lambda_, int grid_size_, int image_res_) : sigma(sigma_), lambda(lambda_), filters(grid_size_ * grid_size_), resolution(image_res_), w(false, grid_size_ * grid_size_, image_res_, image_res_), diff(true, filters, resolution, resolution) {};
    explicit Model(const char subfigure, int grid_size_, int image_res_) : sigma(1.0), lambda(0.5), filters(grid_size_ * grid_size_), resolution(image_res_), w(false, grid_size_ * grid_size_, image_res_, image_res_), diff(true, filters, resolution, resolution) {this->load(subfigure);};
    void update(SquareArray<T> const &x);

    void save(const char &subfigure);
    bool load(const char &subfigure);

private:
    double f(int i, SquareArray<T> const &x);
    CubeArray<T> diff;
};


#endif //FILTER_FINDER_MODEL_H
