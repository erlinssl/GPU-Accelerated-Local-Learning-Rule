#ifndef FILTER_FINDER_MODEL_H
#define FILTER_FINDER_MODEL_H


#include <memory>
#include <string>
#include <filesystem>
#include <array>
#include <cmath>

#include <arrayfire.h>


template <typename T>
class Model {
public:
    af::array mu;

    double sigma;
    double lambda;
    double learning_rate;
    int filters;
    int resolution;

    explicit Model(double sigma_, double lambda_, int grid_size_, int image_res_, double learning_rate_ = 0.1) : sigma(sigma_), lambda(lambda_), filters(grid_size_ * grid_size_), resolution(image_res_), learning_rate(learning_rate_), mu(af::randu(image_res_, image_res_, grid_size_ * grid_size_)){};
    explicit Model(const char subfigure, int grid_size_, int image_res_) : sigma(1.0), lambda(0.5), filters(grid_size_ * grid_size_), resolution(image_res_), mu(af::randu(image_res_, image_res_, grid_size_ * grid_size_)) {this->load(subfigure);};
    void update(af::array const &x);

    void save(char subfigure);
    bool load(char subfigure);

private:
    double f(af::array const &i, af::array const &x, af::seq & pos);
};


#endif //FILTER_FINDER_MODEL_H
