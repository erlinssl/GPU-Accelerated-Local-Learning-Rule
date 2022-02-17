//
// Created by ingebrigt on 27.01.2022.
//

#ifndef FILTER_FINDER_ARRAYS_H
#define FILTER_FINDER_ARRAYS_H


#include <array>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>


template <typename T>
class SquareArray {
public:
    std::vector<T> arr;
    size_t nrows;
    size_t ncols;

    SquareArray(size_t nrows_, size_t ncols_);
    explicit SquareArray(std::vector<T> x);

    void flat(std::vector<float> &out);
    std::vector<std::vector<T>> get_slices(size_t outer_from, size_t outer_to, size_t inner_from, size_t inner_to);
    friend SquareArray<T> operator+(T x, SquareArray<T> y);
    friend SquareArray<T> operator*(T x, SquareArray<T> y);
    // todo does this need to be friend?
    std::vector<T> operator[](size_t i) const;
    std::vector<T> & operator[](size_t i);

    SquareArray<T> operator*(T y);
    SquareArray<T> operator-(SquareArray<T> x);
    SquareArray<T> operator-(SquareArray<T> x) const;
    SquareArray<T> operator-(std::vector<std::vector<T>> y);
    SquareArray<T> operator+(T x);

    SquareArray<T> operator+=(SquareArray<T> y);
    SquareArray<T> operator-=(SquareArray<T> y);


    size_t size() const;
    size_t length() const;
    size_t index(size_t x, size_t y) const;
    void print() const;
};

template <typename T>
class CubeArray {
public:
    std::vector<T> cube;
    size_t nlays;
    size_t nrows;
    size_t ncols;

    CubeArray(bool zero, size_t outer, size_t middle, size_t inner);
    explicit CubeArray(std::vector<std::vector<std::vector<T>>> cube_);

    double calc(SquareArray<T> x, size_t outer);
    SquareArray<T> operator[](size_t i) const; //

    CubeArray<T> operator/(T y);
    CubeArray<T> operator*(T y);

    CubeArray<T> operator+=(CubeArray<T> y);

    friend CubeArray<T> operator*(T y, CubeArray<T> x);

    size_t size();
    size_t length();
    size_t index(size_t x, size_t y, size_t z);
    void print() const;
};


#endif //FILTER_FINDER_ARRAYS_H
