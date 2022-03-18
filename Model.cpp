//
// Created by ingebrigt on 21.01.2022.
//

#include "Model.h"

#include <utility>
#include <fstream>
#include <iterator>

#define DELIMITER ' '


double learning_rate = 0.1;

template <typename T>
std::vector<std::vector<T>> operator-=(af::array &x, af::array &y) {
    x -= y;
    return x;
}

template <typename T>
double Model<T>::f(int i, af::array const &x) {
    return std::exp(-af::sum<double>(af::pow(x - this->mu(i, af::span, af::span), 2))/this->sigma);
}

template <typename T>
void Model<T>::update(af::array const &x) {
    diff = 0;

    //TODO Research parallelization
    for (int i1 = 0; i1 < filters; ++i1) {
        diff(i1, af::span, af::span) += (x - mu(i1, af::span, af::span)) * f(i1, x);

        for (int i2 = 0; i2 < filters; ++i2) {
            if (i1 != i2) {
                diff(i1, af::span, af::span) -= (mu(i1, af::span, af::span) - mu(i2, af::span, af::span)) * 2 * lambda * f(i1, mu(i2, af::span, af::span));
            }
        }

        mu += ((diff * learning_rate) / sigma);
    }
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

    std::cout << "Saving figure to " << path << std::endl;
    af::saveArray(&subfigure, mu, &path[0], false);
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

    std::cout << "Reading array from " << path << std::endl;
    mu = af::readArray(&path[0], subfigure);
    return true;
}

template class Model<int>;
template class Model<double>;
