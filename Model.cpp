//
// Created by ingebrigt on 21.01.2022.
//

#include "Model.h"

#include <utility>
#include <fstream>
#include <iterator>
#include <boost/compute/algorithm/transform.hpp>

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
static double test = 0;
template <typename T>
void Model<T>::update(SquareArray<T> const &x) {
    compute::copy(x.arr.begin(), x.arr.end(), xgpu.begin(), queue);
    //todo vet ikke om meg på sette mgpu som argument hver gang for å få de verdiene som blir oppdatert i metoden??
    //compute::fill(diff2.begin(), diff2.end(), 0, queue);

    using compute::uint_;
    queue.enqueue_1d_range_kernel(kernel,0,16,0);
}


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
        //std::replace(line.begin(), line.end(), '.', ',');
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
            double f(int i, double sigma, __global double *mun, __global double *xv) {
                double sum = 0;
                int nrows = 5;
                int ncols = 5;
                for (int j = 0; j < 5; ++j) {
                    for (int k = 0; k < ncols; ++k) {
                        sum += pow((xv[(j * nrows) + k] - mun[i * 5 * 5 + (j * nrows) + k]), 2);
                    }
                }
                sum = -sum;
                sum /= sigma;
                sum = exp(sum);
                return sum;
            }
            __kernel void SMA(__global double *mu, int filter_size, double lambda, double sigma, __local double *diff, __global double *x_vec) {
                int i1 = get_global_id(0);
                for (int j = 0; j < 5*5; ++j) {
                    diff[i1 * 25 + j] = (x_vec[j] - mu[i1 * 5 * 5 + j]) * f(i1, sigma, mu, x_vec);
                }
                for(int i2 = 0; i2 < filter_size; ++i2) {
                    if (i1 != i2) {
                        for (int k = 0; k < 5 * 5; ++k) {
                            diff[i1 * 25 + k] -= 2.0 * lambda * (mu[i2 * 5 * 5 + k] - mu[i1 * 5 * 5 + k]) * f(i1, sigma, mu, &mu[i2 * 5 * 5]);
                        }
                    }
                }
                for (int i = 0; i < 5 * 5; ++i) {
                    mu[i1 * 25 + i] += diff[i1 * 25 + i] * 0.1 / 1.0;
                }
            }
    );
    // create sma program
    return compute::program::build_with_source(source,context);
}

template class Model<int>;
template class Model<double>;
