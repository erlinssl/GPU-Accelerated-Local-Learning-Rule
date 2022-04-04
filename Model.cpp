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
double Model<T>::f(af::array const &i, af::array const &x) {
    af::array a = (x - i);

    auto temp = af::exp(
            -(af::sum<double>(
                    af::pow2((x - i)(af::span, af::span, 0, 0))
            )
            )/sigma).scalar<float>();
    if((int) std::abs(temp) != 0){
        std::cout << temp << std::endl;
    }
    return temp;
}

template <typename T>
void Model<T>::update(af::array const &x) {
    af::array diff = af::constant(0, resolution, resolution);

    af::array nums = af::seq(filters);
    try {
        gfor(af::seq i1, static_cast<double>(filters))  {
            // todo could probably be optimized, dont know if this is still vectorized
            // af::array temp = mu(af::span, af::span, (int) nums(i1).scalar<float>()); // i1 is always 0
            af::array temp = mu(af::span, af::span, i1); // weird shape 5x5x1x16
            diff += ((x - temp) * f(temp, x));

            for (int i2 = 0; i2 < filters; ++i2) {
                af::array condition = (i1 != i2);
                if((bool) condition.as(f32).scalar<float>()) {
                    diff -= ((mu(af::span, af::span, i2) - temp) * (2.0 * lambda * f(temp, mu(af::span, af::span, i2))));
                }
            }
            mu(af::span, af::span, i1) += ((diff * learning_rate) / sigma);
        }
    } catch (af::exception & e) {
        std::cout << e.what() << std::endl;
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
