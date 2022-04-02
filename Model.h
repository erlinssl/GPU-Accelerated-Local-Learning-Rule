//
// Created by ingebrigt on 21.01.2022.
//

#ifndef FILTER_FINDER_MODEL_H
#define FILTER_FINDER_MODEL_H


#include <memory>
#include <string>
#include "Arrays.h"
#include <filesystem>
#include <boost/compute/system.hpp>
#include <boost/compute/utility/source.hpp>

#include <boost/program_options.hpp>
#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/container/vector.hpp>

namespace compute = boost::compute;
namespace po = boost::program_options;

template <typename T>
class Model {
public:
    CubeArray<T> w;
    double sigma;
    double lambda;
    int filters;
    int resolution;
    compute::vector<double> mugpu;
    compute::command_queue queue;
    explicit Model(double sigma_, double lambda_, int grid_size_, int image_res_) : sigma(sigma_), lambda(lambda_), filters(grid_size_ * grid_size_), resolution(image_res_), w(false, grid_size_ * grid_size_, image_res_, image_res_), diff(true, filters, resolution, resolution) {
        context = compute::context(device);
        program = make_sma_program(context);
        mugpu = compute::vector<double>(w.length(),context);
        queue = compute::command_queue(context, device);
        compute::copy(w.cube.begin(), w.cube.end(), mugpu.begin(), queue);
        xgpu = compute::vector<double>(25,context);
        kernel = compute::kernel(program, "SMA");
        /*
        A(__global double *mu, int filter_size, double lambda, double sigma, __local double *diff, __global double *x_vec) {
        */
        kernel.set_arg(0,mugpu.get_buffer());
        kernel.set_arg(1,filters);
        kernel.set_arg(2,lambda);
        kernel.set_arg(3,sigma);
        clSetKernelArg(kernel, 4, w.length() * sizeof(double), NULL);
    };
    void update(SquareArray<T> const &x);

    void save(const char &subfigure);
    bool load(const char &subfigure);

private:
    compute::kernel kernel;
    compute::program make_sma_program(const compute::context& context);
    compute::device device = compute::system::default_device();
    compute::context context;
    compute::program program;
    compute::vector<double> xgpu;
    double f(int i, SquareArray<T> const &x);
    CubeArray<T> diff;
};


#endif //FILTER_FINDER_MODEL_H
