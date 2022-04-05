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
double Model<T>::f(af::array const &i, af::array const &x, af::seq & pos) {
    /*
    af::array pow = af::pow((x-i), 2);
    auto summation = af::moddims(pow, resolution, resolution, filters);
    af_print(summation);
    auto summed = af::sum(af::sum(summation));
    af_print(summed);
    std::cout << "sum " << summed(pos).scalar<float>() << std::endl;
    sleep(5);
     */

    /*auto temp = af::exp(
            -(af::sum<double>(
                    af::pow((x - i), 2)
            )
            )/sigma).scalar<float>();
    return temp;*/
    return af::exp(-af::sum(af::sum(af::moddims(af::pow(x-i, 2), resolution, resolution, filters)))/sigma).scalar<float>();
}

template <typename T>
void Model<T>::update(af::array const &x) {
    // TODO Occasionally returns sigsegv in first batch
    af::array diff = af::constant(0, resolution, resolution, filters);

    af::array nums = af::seq(filters);
    try {
        gfor(af::seq i1, filters)  {
            // todo could probably be optimized, dont know if this is still vectorized
            // af::array temp = mu(af::span, af::span, (int) nums(i1).scalar<float>()); // i1 is always 0

            diff(af::span, af::span, i1) += ((x - mu(af::span, af::span, i1)) * f(mu(af::span, af::span, i1), x, i1));

            for (int i2 = 0; i2 < filters; ++i2) {
                diff(af::span, af::span, i1) -= ((mu(af::span, af::span, i2) - mu(af::span, af::span, i1)) * (2.0 * lambda * f(mu(af::span, af::span, i1), mu(af::span, af::span, i2), i1))) * (i1 == i2);
            }
                // TODO Try to access diff(span, span, 0, i1) from the [5, 5, 1, 16] structure ? \
                    Might not be necessary ?
            // std::cout << diff.dims() << std::endl;
            // std::cout << diff(af::span, af::span, 0, i1).dims() << std::endl;
        }
        // af_print(diff);
        mu += ((diff * learning_rate) / sigma);
    } catch (af::exception & e) {
        std::cout << "\033[1;31m" << e.what() << "\033[0m" << std::endl;
        exit(1);
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
void Model<T>::save(const char subfigure) {
    std::string path = "../saved/figure2";
    path.push_back(subfigure);
    path.append(".fig");

    std::cout << "Saving figure to " << path << std::endl;
    auto temp = af::saveArray(&subfigure, mu, &path[0], false);
    std::cout << "index of array '" << &subfigure << "' is " << temp << std::endl;
}

template <typename T>
bool Model<T>::load(const char subfigure) {
    std::string path = "../saved/figure2";
    path.push_back(subfigure);
    path.append(".fig");
    if(!std::filesystem::exists(path)){
        std::cout << "Figure " << subfigure << " not found." << std::endl;
        return false;
    }

    std::cout << "Reading array with key '" << &subfigure << "' from " << path << std::endl;
    auto index = af::readArrayCheck(&path[0], &subfigure);
    if(index >= 0) {
        mu = af::readArray(&path[0], index);
    } else {
        std::cout << "Invalid key" << std::endl;
        return false;
    }
    return true;
}

template class Model<int>;
template class Model<float>;
template class Model<double>;
