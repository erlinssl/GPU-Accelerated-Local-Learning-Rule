//
// Created by ingebrigt on 27.01.2022.
//

#ifndef FILTER_FINDER_ARRAYS_H
#define FILTER_FINDER_ARRAYS_H


#include <array>
#include <cmath>
#include <vector>


class SquareArray {
    std::vector<std::vector<double>> arr;
};

class CubeArray {
public:
    CubeArray(bool zero, int outer, int middle, int inner);
    std::vector<std::vector<std::vector<double>>> w;
    double calc(std::vector<std::vector<double>> x, size_t outer);
};


#endif //FILTER_FINDER_ARRAYS_H
