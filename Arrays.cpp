//
// Created by ingebrigt on 27.01.2022.
//

#include "Arrays.h"

#include <utility>

// -----------------------------------------------------------------------
// ----------------------------- CUBE ARRAYS -----------------------------
// -----------------------------------------------------------------------

template <typename T>
CubeArray<T>::CubeArray(bool zero, int outer, int middle, int inner) {
    for (int i = 0; i < outer; ++i) {
        cube.emplace_back(std::vector<std::vector<T>>());
        for (int j = 0; j < middle; ++j) {
            cube[i].emplace_back(std::vector<T>());
            for (int k = 0; k < inner; ++k) {
                if (zero) cube[i][j].emplace_back(0);
                    // todo use c++ 11 instead
                    // todo fix this concerning templates
                else cube[i][j].emplace_back(((T) rand() / RAND_MAX) * (1 - 0));
            }
        }
    }
}

template <typename T>
CubeArray<T>::CubeArray(std::vector<std::vector<std::vector<T>>> cube_) {
    cube = std::move(cube_);
}


template <typename T>
double CubeArray<T>::calc(SquareArray<T> x, size_t outer) {
    double sum = 0;
    for (size_t inner = 0; inner < 5; inner++) {
        for (size_t index = 0; index < 5; index++) {
            double value = x[inner][index] - this->cube[outer][inner][index];
            sum += std::pow(value, 2);
        }
    }
    return -sum;
}

template <typename T>
SquareArray<T> CubeArray<T>::operator[](size_t i) const {
    return SquareArray(cube[i]);
}

template <typename T>
CubeArray<T> operator*(T y, CubeArray<T> x) {
    for (int i = 0; i < x.size(); ++i) {
        for (int j = 0; j < x[i].size(); ++j) {
            for (int k = 0; k < x[i][j].size(); ++k) {
                x[i][j][k] *= y;
            }
        }
    }
    return x;
}

template <typename T>
size_t CubeArray<T>::size() {
    return cube.size();
}

template <typename T>
CubeArray<T> CubeArray<T>::operator/(T y) {
    auto x = *this;
    for (int i = 0; i < x.size(); ++i) {
        for (int j = 0; j < x[i].size(); ++j) {
            for (int k = 0; k < x[i][j].size(); ++k) {
                x[i][j][k] /= y;
            }
        }
    }
    return x;
}

template <typename T>
CubeArray<T> CubeArray<T>::operator*(T y) {
    auto x = *this;
    for (int i = 0; i < x.size(); ++i) {
        for (int j = 0; j < x[i].size(); ++j) {
            for (int k = 0; k < x[i][j].size(); ++k) {
                x[i][j][k] *= y;
            }
        }
    }
    return x;
}

template <typename T>
CubeArray<T> CubeArray<T>::operator+=(CubeArray<T> y) {
    for (int i = 0; i < this->size(); ++i) {
        for (int j = 0; j < (*this)[i].size(); ++j) {
            for (int k = 0; k < (*this)[i][j].size(); ++k) {
                (*this)[i][j][k] += y[i][j][k];
            }
        }
    }
    return *this;
}

template <typename T>
void CubeArray<T>::print() {
    for(auto square : cube){
        for(auto vec : square){
            for(auto value : vec){
                std::cout << value << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

// -----------------------------------------------------------------------
// ---------------------------- SQUARE ARRAYS ----------------------------
// -----------------------------------------------------------------------

template <typename T>
SquareArray<T>::SquareArray(std::vector<std::vector<T>> x) {
    arr = x;
}

template <typename T>
size_t SquareArray<T>::size() {
    return arr.size();
}


template <typename T>
SquareArray<T>::SquareArray(int outer, int inner) {
    arr = std::vector<std::vector<T>>();
    for (int i = 0; i < outer; ++i) {
        arr.emplace_back(std::vector<T>());
        for (int j = 0; j < inner; ++j) {
            // todo use c++11 instead
            arr[j].emplace_back(((T) rand() / RAND_MAX) * (1 - 0));
        }
    }
}

template <typename T>
std::vector<T> SquareArray<T>::operator[](size_t i) {
    return arr[i];
}

template <typename T>
SquareArray<T> operator*(T x, SquareArray<T> y) {
    for (int i = 0; i < y.size(); i++) {
        for (auto &val : y[i]) {
            val *= x;
        }
    }
    return y;
}

template <typename T>
SquareArray<T> SquareArray<T>::operator-(std::vector<std::vector<T>> y) {
    for (int i = 0; i < size(); ++i) {
        for (int j = 0; j < arr[i].size(); ++j) {
            (*this)[i][j] = (*this)[i][j] - y[i][j];
        }
    }
    return *this;
}

template <typename T>
SquareArray<T> SquareArray<T>::operator+=(SquareArray<T> y) {
    for (int i = 0; i < size(); ++i) {
        for (int j = 0; j < y.size(); ++j) {
            arr[i][j] += y[i][j];
        }
    }
    return SquareArray(*this);
}

template <typename T>
SquareArray<T> SquareArray<T>::operator-=(SquareArray<T> y) {
    for (int i = 0; i < size(); ++i) {
        for (int j = 0; j < y.size(); ++j) {
            arr[i][j] -= y[i][j];
        }
    }
    return SquareArray(*this);
}

template <typename T>
SquareArray<T> SquareArray<T>::operator-(SquareArray<T> y) {
    auto x = *this;
    for (int i = 0; i < x.size(); ++i) {
        for (int j = 0; j < x[i].size(); ++j) {
            x[i][j] -= y[i][j];
        }
    }
    return x;
}

template <typename T>
SquareArray<T> SquareArray<T>::operator+(T y) {
    auto x = *this;
    for (int i = 0; i < x.size(); ++i) {
        for (int j = 0; j < x[i].size(); ++j) {
            x[i][j] += y;
        }
    }
    return x;
}

template <typename T>
SquareArray<T> SquareArray<T>::operator*(T y) {
    auto x = *this;
    for (int i = 0; i < x.size(); ++i) {
        for (int j = 0; j < x[i].size(); ++j) {
            x[i][j] *= y;
        }
    }
    return x;
}

template <typename T>
void SquareArray<T>::flat(std::vector<float> &out) {
    for (size_t i = 0; i < size(); i++){
        for (size_t j = 0; j < size(); j++){
            out.at(size() * i + j) = arr[i][j];
        }
    }
    // return out;
}

template <typename T>
void SquareArray<T>::print(){
    for(const auto& vec : arr){
        for(auto& value : vec){
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
}

template <typename T>
SquareArray<T> operator+(T x, SquareArray<T> y) {
    for (int i = 0; i < y.size(); ++i) {
        for (int j = 0; j < y[i].size(); ++j) {
            y[i][j] += x;
        }
    }
    return y;
}

template class CubeArray<double>;
template class CubeArray<int>;
template class SquareArray<double>;
template class SquareArray<int>;
