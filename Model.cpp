//
// Created by ingebrigt on 21.01.2022.
//

#include "Model.h"

#include <utility>
#include <fstream>
#include <iterator>

#define DELIMITER ' '


namespace compute = boost::compute;
namespace po = boost::program_options;
double learning_rate = 0.1;

template <typename T>
std::vector<std::vector<T>> operator-=(std::vector<std::vector<double>> &x, SquareArray<T> y) {
    for (int i = 0; i < x.size(); ++i) {
        for (int j = 0; j < x[i].size(); ++j) {
            x[i][j] -= y[i][j];
        }
    }
    return x;
}


template <typename T>
double Model<T>::f(int i, SquareArray<T> const &x) {
    return std::exp(this->w.calc(x, i)/this->sigma);
}
static int coun = 0;
template <typename T>
void Model<T>::update(SquareArray<T> const &x) {
    std::cout << "update: " << coun++ << std::endl;
    compute::vector<double> mugpu(w.length(),context);
    compute::vector<double> xgpu(x.length(),context);
    compute::vector<double> ans(1, context);
    compute::command_queue queue(context, device);
    std::vector<double> a(w.length());
    compute::copy(w.cube.begin(), w.cube.end(), mugpu.begin(), queue);
    compute::copy(x.arr.begin(), x.arr.end(), xgpu.begin(), queue);
    compute::kernel kernel(program, "SMA");
    /*
    A(__global double *mu, int filter_size, double lambda, double sigma, __local double *diff, __global double *x_vec) {
    */
     kernel.set_arg(0,mugpu.get_buffer());
     kernel.set_arg(1,filters);
     kernel.set_arg(2,lambda);
     kernel.set_arg(3,sigma);
     clSetKernelArg(kernel, 4, w.length() * sizeof(double), NULL);
     kernel.set_arg(5,xgpu.get_buffer());

     using compute::uint_;
     uint_ tpb = 128;
     uint_ workSize = filters;
     queue.enqueue_1d_range_kernel(kernel,0,workSize,tpb);
     compute::copy(mugpu.begin(), mugpu.end(), w.cube.begin(), queue);
}

/* Saved array
 * 1 2
 * 3 4
 *
 * 5 6
 * 7 8
 *
 * 3 2
 * 4 1
 *
 * represents (3x2x2) array
 *
 * [[[1 2],
 *   [3 4] ],
 *
 *  [[5 6],
 *   [7 8] ],
 *
 *  [[3 2],
 *   [4 1] ]]
*/

template <typename T>
void Model<T>::save(const char &subfigure) {
    std::string path = "../saved/figure2";
    path.push_back(subfigure);
    path.append(".fig");
    std::ofstream output_file(path);

    std::cout << "Saving figure" << std::endl;

    for(size_t layer = 0; layer < filters; layer++) {
        for (size_t row = 0; row < resolution; row++) {
            auto temp2 = w[layer];
            auto temp = temp2[row];
            std::ostream_iterator<double> output_iterator(output_file, " ");
            std::copy(temp.begin(), temp.end(), output_iterator);
            output_file << "\n";
        }
        output_file << "\n";
    }

    /*
    for (int x = 0; x < filters; ++x) {
        for (int y = 0; y < resolution; ++y) {
            std::ostream_iterator<double> output_iterator(output_file, " ");
            std::copy(w.cube.begin(), w.cube.begin() + resolution, output_iterator);
            output_file << "\n";
        }
        output_file << "\n";
    }
    */
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

template <typename T>
bool Model<T>::load(const char &subfigure) {
    std::string path = "../saved/figure2";
    path.push_back(subfigure);
    path.append(".fig");
    if(!std::filesystem::exists(path)){
        std::cout << "Figure " << subfigure << " not found." << std::endl;
        return false;
    }
    std::ifstream file(path);

    std::string line;
    this->w.cube.clear();

    std::vector<T> inner = {};

    while (std::getline(file, line)) {
        rtrim(line);
        // TODO The following line may or may not need to be active, depending on system locale \
            If filter plots are empty, try (un)commenting it.
        std::replace(line.begin(), line.end(), '.', ',');
        size_t last = 0, next;
        while ((next = line.find(DELIMITER, last)) != std::string::npos) {
            inner.emplace_back(std::stod(line.substr(last, next-last)));
            last = next + 1;
        }
        if(!line.substr(last).empty()){
            inner.emplace_back(std::stod(line.substr(last)));
        }
    }

    this->w.cube.swap(inner);
    return true;
}

template<typename T>
compute::program Model<T>::make_sma_program(const compute::context &context) {
    const char source[] = BOOST_COMPUTE_STRINGIZE_SOURCE (
            double f(int i, int filter_size, double sigma, __global double *mun, __global double *xv) {
                double sum = 0;
                int nrows = 5;
                int ncols = 5;
                for (int j = 0; j < 5; ++j) {
                    for (int k = 0; k < ncols; ++k) {
                        sum += (xv[(j * nrows) + k] - mun[(i * nrows * ncols) + (j * nrows) + k]) *
                               (xv[(j * nrows) + k] - mun[(i * nrows * ncols) + (j * nrows) + k]);
                    }
                }
                sum = -sum;
                sum /= sigma;
                sum = exp(sum);
                return sum;
            }
            //todo fix math
            __kernel void SMA(__global double *mu, int filter_size, double lambda, double sigma, __local double *diff, __global double *x_vec) {
                double learning_rate = 0.1;
                // Store each work-item's unique row and column
                int x = get_global_id(0);
                int y = get_global_id(5);
                for (int i = 0; i < filter_size * 5 * 5; ++i) {
                    diff[i] = 0;
                }
                // Iterate the filter rows
                for (int i = 0; i < filter_size; i++) {
                    for (int j = 0; j < filter_size; ++j) {
                        diff[filter_size * i + j] += (x_vec[j] - mu[i * filter_size + j]) * f(i, filter_size, sigma, &mu[i * filter_size], x_vec);
                    }
                    for (int j = 0; j < filter_size; j++) {
                        if (i != j) {
                            for (int k = 0; k < filter_size * filter_size; ++k) {
                                diff[filter_size * i + k] -= 2.0 * lambda * (mu[j * filter_size + k] - mu[i * filter_size + k]) * f(i, filter_size, sigma, &mu[j * filter_size], x_vec);
                            }
                        }
                    }
                }
                for (int i = 0; i < filter_size * 5 * 5; ++i) {
                    mu[i] += learning_rate * diff[i] / sigma;
                }
            }
    );
    // create sma program
    return compute::program::build_with_source(source,context);
}

template class Model<int>;
template class Model<double>;
