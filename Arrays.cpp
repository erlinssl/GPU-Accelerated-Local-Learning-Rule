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
    // TODO Find a less primitive solution
    nrows = std::sqrt(x.size());
    ncols = std::sqrt(x.size());
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
SquareArray<T> SquareArray<T>::operator-(SquareArray<T> y) const {
    auto x = *this;
    for (size_t i = 0; i < arr.size(); ++i) {
        x.arr[i] -= y.arr[i];
    }
    return x;
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
    for (size_t i = 0; i < size(); i++){
        for (size_t j = 0; j < size(); j++){
            out[size() * i + j] = arr[index(i, j)];
        }
    }
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
    for (size_t i = outer_from; i < outer_to; ++i) {
        ans.emplace_back(std::vector<T>());
        for (size_t j = inner_from; j < inner_to; ++j) {
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
size_t SquareArray<T>::index(size_t x, size_t y) const {
    return (x * nrows) + y;
}

template <typename T>
void SquareArray<T>::print() const {
    size_t counter = 0;
    for(const auto& value : arr){
        std::cout << value << " ";
        counter++;
        if(counter % ncols == 0){
            std::cout << std::endl;
        }
    }
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// ----------------------------- CUBE ARRAYS -----------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------



template <typename T>
CubeArray<T>::CubeArray(bool zero, size_t outer, size_t middle, size_t inner) {
    for (size_t i = 0; i < outer*middle*inner; ++i){
        if(zero){
            cube.emplace_back(0);
        } else {
            // todo use c++ 11 instead
            cube.emplace_back(((T) rand() / RAND_MAX) * (1 - 0));
        }
    }
}

template <typename T>
CubeArray<T>::CubeArray(std::vector<std::vector<std::vector<T>>> cube_) {
    nlays = cube_.size();
    nrows = cube_[0].size();
    ncols = cube_[0][0].size();

    std::vector<T> temp;
    for (auto & layer : cube_) {
        for (auto & row : layer) {
            for (auto & col : row) {
                temp.push_back(col);
            }
        }
    }
    cube.swap(temp);
}


template <typename T>
double CubeArray<T>::calc(SquareArray<T> x, size_t outer) {
    double sum = 0;

    for (size_t row = 0; row < nrows; row++) {
        for (size_t value = 0; value < ncols; value++) {
            sum += std::pow(
                    x.arr[x.index(row, value)] - this->cube[index(outer, row, value)]
                    , 2);
        }
    }
    return -sum;
}

template <typename T>
SquareArray<T> CubeArray<T>::operator[](size_t i) const {
    std::vector<T> temp;
    temp.reserve(nrows*ncols);
    for(size_t j = 0; j < nrows * ncols; ++j){
        temp.push_back(cube[i*nrows*ncols + j]);
    }
    return SquareArray(temp);
}

template <typename T>
CubeArray<T> operator*(T y, CubeArray<T> x) {
    for(auto & val : x.cube){
        val *= y;
    }
    return x;
}

template <typename T>
CubeArray<T> CubeArray<T>::operator/(T y) {
    auto x = *this;
    for (auto & val : x.cube) {
        val /= y;
    }
    return x;
}

template <typename T>
CubeArray<T> CubeArray<T>::operator*(T y) {
    auto x = *this;
    for (auto & val : x.cube) {
        val *= y;
    }
    return x;
}

template <typename T>
CubeArray<T> CubeArray<T>::operator+=(CubeArray<T> y) {
    for (size_t i = 0; i < length(); i++){
        cube[i] += y.cube[i];
    }
    return *this;
}

template <typename T>
size_t CubeArray<T>::size() {
    return nlays;
}

template <typename T>
size_t CubeArray<T>::length() {
    return cube.size();
}

template <typename T>
size_t CubeArray<T>::index(size_t x, size_t y, size_t z){
    return (x * nlays * nrows) + (y * nrows) + z;
}

template <typename T>
void CubeArray<T>::print() const {
    size_t counter = 0;
    size_t rows = 1;
    for(auto & val : cube){
        std::cout << val << " ";
        counter++;
        if (counter % ncols == 0){
            std::cout << std::endl;
            rows++;
        }
        if (rows % (nrows+1) == 0){
            std::cout << std::endl;
            counter = 0;
            rows = 1;
        }
    }
}


template class SquareArray<double>;
template class SquareArray<int>;
template class CubeArray<double>;
template class CubeArray<int>;
