//
// Created by ingebrigt on 21.01.2022.
//

#include "Model.h"

#include <utility>
#include <fstream>
#include <iterator>

#define DELIMITER ' '


template <typename T>
std::vector<std::vector<T>> operator-=(std::vector<std::vector<double>> &x, SquareArray<T> y) {
    for (int i = 0; i < x.size(); ++i) {
        for (int j = 0; j < x[i].size(); ++j) {
            x[i][j] -= y[i][j];
        }
    }
    return x;
}


template <typename T>
double Model<T>::f(int i, SquareArray<T> const &x) {
    return std::exp(this->w.calc(x, i)/this->sigma);
}

template <typename T>
void Model<T>::update(SquareArray<T> const &x) {
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    std::vector<CubeArray<T>> diffs;
    diffs.reserve(num_threads);

    int start = 0;
    int thread_diff = filters / num_threads;
    thread_diff += filters % num_threads;
    for (int i = 0; i < num_threads; ++i) {
        int end = start + thread_diff;
        //std::cout << "start: " << start << " - end: " << end << std::endl;
        threads.push_back(std::thread([this, start, end, &diffs, x]{
            CubeArray<T> diff(true, filters, resolution, resolution);
            for (size_t i1 = start; i1 < end; ++i1) {
                diff.plus_index(i1, (x - w[i1]) * (f(i1, x)));

                for (size_t i2 = 0; i2 < filters; ++i2) {
                    if (i1 != i2) {
                        diff.minus_index(i1, (w[i2] - w[i1]) * (2.0 * lambda * f(i1, w[i2])));
                    }
                }
            }
            diffs.emplace_back(std::move(diff));
        }));

        start = end;
        thread_diff = filters / num_threads;
    }

    for (size_t j = 0; j < num_threads; ++j) {
        threads[j].join();
    }

    for (size_t j = 1; j < num_threads; ++j) {
        diffs[0] += diffs[j];
    }

    w += (((diffs[0]/num_threads) * learning_rate) / sigma);
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
void Model<T>::save(const char &subfigure) {
    std::string path = "../saved/figure2";
    path.push_back(subfigure);
    path.append(".fig");
    std::ofstream output_file(path);

    std::cout << "Saving figure" << std::endl;

    for(size_t layer = 0; layer < filters; layer++) {
        for (size_t row = 0; row < resolution; row++) {
            auto temp2 = w[layer];
            auto temp = temp2[row];
            std::ostream_iterator<double> output_iterator(output_file, " ");
            std::copy(temp.begin(), temp.end(), output_iterator);
            output_file << "\n";
        }
        output_file << "\n";
    }

    /*
    for (int x = 0; x < filters; ++x) {
        for (int y = 0; y < resolution; ++y) {
            std::ostream_iterator<double> output_iterator(output_file, " ");
            std::copy(w.cube.begin(), w.cube.begin() + resolution, output_iterator);
            output_file << "\n";
        }
        output_file << "\n";
    }
    */
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

template <typename T>
bool Model<T>::load(const char &subfigure) {
    std::string path = "../saved/figure2";
    path.push_back(subfigure);
    path.append(".fig");
    if(!std::filesystem::exists(path)){
        std::cout << "Figure " << subfigure << " not found." << std::endl;
        return false;
    }
    std::ifstream file(path);

    std::string line;
    this->w.cube.clear();

    std::vector<T> inner = {};

    while (std::getline(file, line)) {
        rtrim(line);
        // TODO The following line may or may not need to be active, depending on system locale \
            If filter plots are empty, try (un)commenting it.
        //std::replace(line.begin(), line.end(), '.', ',');
        size_t last = 0, next;
        while ((next = line.find(DELIMITER, last)) != std::string::npos) {
            inner.emplace_back(std::stod(line.substr(last, next-last)));
            last = next + 1;
        }
        if(!line.substr(last).empty()){
            inner.emplace_back(std::stod(line.substr(last)));
        }
    }

    this->w.cube.swap(inner);
    return true;
}

template class Model<int>;
template class Model<double>;
