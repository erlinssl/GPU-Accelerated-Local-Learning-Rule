//
// Created by ingebrigt on 27.01.2022.
//

// TODO Refactor usages of 2D arrays


#include "Arrays.h"

#include <utility>


// -----------------------------------------------------------------------
// ---------------------------- SQUARE ARRAYS ----------------------------
// -----------------------------------------------------------------------

template <typename T>
SquareArray<T>::SquareArray(std::vector<T> x) {
    arr = x;
}


template <typename T>
SquareArray<T>::SquareArray(size_t nrows_, size_t ncols_) {
    nrows = nrows_;
    ncols = ncols_;
    arr = std::vector<T>();
    for (int i = 0; i < nrows_*ncols_; ++i){
        // todo use c++11 instead
        arr.emplace_back(((T) rand() / RAND_MAX) * (1 - 0));
    }
}

/*
template <typename T>
std::vector<T> SquareArray<T>::operator[](size_t i) {
    return std::vector<T>(arr.begin() + nrows * i, arr.begin() + nrows * i + ncols);
}
 */

// TODO Might need to directly reference the actual vector?
template <typename T>
std::vector<T> & SquareArray<T>::operator[](size_t i) {
    std::vector<T> temp;
    for(size_t j = 0; j < ncols; j++){
        temp.push_back(arr[index(i, j)]);
    }
    return temp;
}

template <typename T>
std::vector<T> SquareArray<T>::operator[](size_t i) const {
    std::vector<T> temp;
    for(size_t j = 0; j < ncols; j++){
        temp.push_back(arr[index(i, j)]);
    }
    return temp;
}

template <typename T>
SquareArray<T> operator*(T x, SquareArray<T> y) {
    for (auto &val : y) {
        val *= x;
    }
    return y;
}

// TODO Change parameter type from 2D to 1D vector, change references to method
template <typename T>
SquareArray<T> SquareArray<T>::operator-(std::vector<std::vector<T>> y) {
    for (int i = 0; i < ncols; ++i) {
        for (int j = 0; j < nrows; ++j) {
            (*this).arr[index(i, j)] = (*this).arr[index(i, j)] - y[i][j];
        }
    }
    return *this;
}

template <typename T>
SquareArray<T> SquareArray<T>::operator-(SquareArray<T> y) {
    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i] -= y.arr[i];
    }
    return *this;
}

template <typename T>
SquareArray<T> SquareArray<T>::operator+=(SquareArray<T> y) {
    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i] += y.arr[i];
    }
    return *this;
}

template <typename T>
SquareArray<T> SquareArray<T>::operator-=(SquareArray<T> y) {
    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i] -= y.arr[i];
    }
    return *this;
}

template <typename T>
SquareArray<T> SquareArray<T>::operator+(T y) {
    for(T & val : arr){
        val += y;
    }
    return *this;
}

template <typename T>
SquareArray<T> SquareArray<T>::operator*(T y) {
    for(T & val : arr){
        val *= y;
    }
    return *this;
}

template <typename T>
void SquareArray<T>::flat(std::vector<float> &out) {
    out = std::vector<float>(arr.begin(), arr.end());
}

template <typename T>
SquareArray<T> operator+(T x, SquareArray<T> y) {
    for (auto & val : y){
        val += x;
    }
    return y;
}

template <typename T>
std::vector<std::vector<T>> SquareArray<T>::get_slices(size_t outer_from, size_t outer_to, size_t inner_from, size_t inner_to) {
    std::vector<std::vector<T>> ans;

    for (int i = outer_from; i < outer_to; ++i) {
        ans.emplace_back(std::vector<T>());
        for (int j = inner_from; j < inner_to; ++j) {
            ans[i - outer_from].emplace_back(arr[index(i, j)]);
        }
    }
    return ans;
}

template <typename T>
size_t SquareArray<T>::size() const {
    return nrows;
}

template <typename T>
size_t SquareArray<T>::length() const {
    return arr.size();
}

template <typename T>
size_t SquareArray<T>::index(size_t x, size_t y) const{
    return x * nrows + y;
}

template <typename T>
void SquareArray<T>::print(){
    size_t counter = 0;
    for(const auto& value : arr){
        std::cout << value << " ";
        counter++;
        if(counter % 5 == 0){
            std::cout << std::endl;
        }
    }
}

// -----------------------------------------------------------------------
// ----------------------------- CUBE ARRAYS -----------------------------
// -----------------------------------------------------------------------

template <typename T>
CubeArray<T>::CubeArray(bool zero, size_t outer, size_t middle, size_t inner) {
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


template class SquareArray<double>;
template class SquareArray<int>;
template class CubeArray<double>;
template class CubeArray<int>;
