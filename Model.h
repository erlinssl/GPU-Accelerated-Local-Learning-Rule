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
    compute::vector<double> batch_data;
    compute::context context;
    compute::command_queue queue;
    compute::program program;
    explicit Model(double sigma_, double lambda_, int grid_size_, int image_res_, int batch_size) : sigma(sigma_), lambda(lambda_), filters(grid_size_ * grid_size_), resolution(image_res_), w(false, grid_size_ * grid_size_, image_res_, image_res_) {
        kernel_options = std::string("-Dfilters=");
        kernel_options.append(std::to_string(filters));
        kernel_options.append(" -Dresolution=");
        kernel_options.append(std::to_string(resolution));
        kernel_options.append(" -Dlambda=");
        kernel_options.append(std::to_string(lambda));
        kernel_options.append(" -Dsigma=");
        kernel_options.append(std::to_string(sigma));
        kernel_options.append(" -Dbatch_size=");
        kernel_options.append(std::to_string(batch_size));


        context = compute::context(device);
        program = make_sma_program(context);
        mugpu = compute::vector<double>(w.length(),context);
        batch_data = compute::vector<double>(25000, context);
        queue = compute::command_queue(context, device);
        compute::copy(w.cube.begin(), w.cube.end(), mugpu.begin(), queue);
        kernel = compute::kernel(program, "SMA");
        /*
        A(__global double *mu, int filter_size, double lambda, double sigma, __local double *diff, __global double *x_vec) {
        */

        //compute::copy(w.cube.begin(), w.cube.end(), mugpu.begin(), queue);
        kernel.set_arg(0,mugpu.get_buffer());
        clSetKernelArg(kernel, 1, 5 * 5 * filters * sizeof(double), NULL);
        kernel.set_arg(2,batch_data.get_buffer());


    };
    void update(int j);

    void save(const char &subfigure);
    bool load(const char &subfigure);

private:
    std::string kernel_options;
    compute::kernel kernel;
    compute::program make_sma_program(const compute::context& context);
    compute::device device = compute::system::default_device();
    double f(int i, SquareArray<T> const &x);
};


#endif //FILTER_FINDER_MODEL_H
