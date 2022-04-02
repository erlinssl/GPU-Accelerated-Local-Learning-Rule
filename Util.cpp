//
// Created by ingebrigt on 19.02.2022.
//

#include <random>

static std::random_device rd;
static std::mt19937 mt(rd());
static std::uniform_real_distribution<double> dist(0.0, 1.0);

static double get_rand() {
    return dist(mt);
}

static size_t index(size_t nrows, size_t x, size_t y) {
    return (x * nrows) + y;
}

