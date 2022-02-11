//
// Created by erlinssl on 11/02/2022.
//

#include <chrono>
#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>

using namespace std::chrono;

class Vector_2{
public:
    std::vector<std::vector<double>> vec;

    Vector_2(size_t outer, size_t inner) {
        vec = std::vector<std::vector<double>>();
        for (size_t i = 0; i < outer; ++i) {
            vec.emplace_back(std::vector<double>());
            for (size_t j = 0; j < inner; ++j) {
                // todo use c++11 instead
                vec[i].emplace_back(((double) rand() / RAND_MAX) * (1 - 0));
            }
        }
    }

    size_t size() {
        return vec.size();
    }

    std::vector<double> operator[](size_t x) {
        return vec[x];
    }

    Vector_2 operator-(Vector_2 other) {
        for (size_t i = 0; i < size(); ++i) {
            for (size_t j = 0; j < vec[i].size(); ++j) {
                (*this)[i][j] = (*this)[i][j] - other[i][j];
            }
        }
        return *this;
    }
};

class Vector_1 {
public:
    std::vector<double> vec;
    size_t nrows;
    size_t ncols;

    Vector_1(size_t nrows_, size_t ncols_) {
        nrows = nrows_;
        ncols = ncols_;
        vec = std::vector<double>();

        for (size_t i = 0; i < nrows_ * ncols_; ++i) {
            // todo use c++11 instead
            vec.emplace_back(((double)rand() / RAND_MAX) * (1 - 0));
        }
    }

    size_t index(size_t x, size_t y) {
        return nrows*x + y;
    }

    double operator[](size_t index){
        return vec[index];
    }

    Vector_1 operator-(Vector_1 other){
        Vector_1 new_vec(nrows, ncols);
        for(size_t i = 0; i < nrows; i++){
            for(size_t j = 0; i < ncols; i++){
                new_vec.vec.push_back(this->vec[index(i, j)] - other[index(i, j)]);
            }
        }
        return new_vec;
    }
};


int main(){
    auto start = high_resolution_clock::now();
    for(size_t i = 0; i < 100*25600; i++){
        Vector_1 vector11(5, 5);
        Vector_1 vector12(5, 5);

        Vector_1 vector13 = vector11 - vector12;
    }
    auto stop = high_resolution_clock::now();
    auto dur = duration_cast<microseconds>(stop - start).count();
    std::cout << "1dim 2D array used " << std::setw(8) << dur << " microseconds with an average of " << std::setw(6) << dur/100 << " per batch" << std::endl;


    start = high_resolution_clock::now();
    for(size_t i = 0; i < 100*25600; i++){
        Vector_2 vector21(5, 5);
        Vector_2 vector22(5, 5);

        Vector_2 vector3 = vector21 - vector22;
    }
    stop = high_resolution_clock::now();
    dur = duration_cast<microseconds>(stop - start).count();
    std::cout << "True 2D array used " << std::setw(8) << dur << " microseconds with an average of " << std::setw(6) << dur/100 << " per batch" << std::endl;

}
