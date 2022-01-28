//
// Created by ingebrigt on 27.01.2022.
//

#ifndef FILTER_FINDER_ARRAYS_H
#define FILTER_FINDER_ARRAYS_H


#include <array>
#include <cmath>
#include <vector>


class SquareArray {
public:
    SquareArray(int outer, int inner);
    SquareArray(std::vector<std::vector<double>> x);
    std::vector<std::vector<double>> arr;
    std::vector<double> operator[](size_t i);
    size_t size();
    SquareArray operator-(std::vector<std::vector<double>> y);
    friend SquareArray operator*(double x, SquareArray y);
    friend SquareArray operator+=(std::vector<std::vector<double>> x, SquareArray y);
    SquareArray operator-(SquareArray x);
    SquareArray operator*(double y);
};

class CubeArray {
public:
    CubeArray(bool zero, int outer, int middle, int inner);
    size_t size();
    std::vector<std::vector<std::vector<double>>> w;
    double calc(SquareArray x, size_t outer);
    SquareArray operator[](size_t i);
    CubeArray operator/(double y);
    CubeArray operator+=(CubeArray y);
    friend CubeArray operator*(double y, CubeArray x);
};


#endif //FILTER_FINDER_ARRAYS_H
