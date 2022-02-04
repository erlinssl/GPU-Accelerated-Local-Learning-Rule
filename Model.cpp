//
// Created by ingebrigt on 21.01.2022.
//

#include "Model.h"

#include <utility>
#include <fstream>

#define DELIMITER ' '


double learning_rate = 0.1;
double Model::f(int i, SquareArray x) {
    return std::exp(this->w.calc(std::move(x), i)/this->sigma);
}

std::vector<std::vector<double>> operator-=(std::vector<std::vector<double>> &x, SquareArray y) {
    for (int i = 0; i < x.size(); ++i) {
        for (int j = 0; j < x[i].size(); ++j) {
            x[i][j] -= y[i][j];
        }
    }
    return x;
}

void Model::update(SquareArray x) {
    auto diff = CubeArray(true, 16, 5, 5);
    // todo make 16 dynamic according to outermost shape
    for (int i1 = 0; i1 < 16; ++i1) {
        diff.cube[i1] += f(i1, x) * (x - w.cube[i1]);
        for (int i2 = 0; i2 < 16; ++i2) {
            if (i1 != i2) {
                diff.cube[i1] -= 2.0 * lambda * (w[i2] - w[i1]) * f(i1, w[i2]);
            }
        }
        w += learning_rate * diff / sigma;
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

void Model::save(const char &subfigure) {
    std::string path = "../saved/figure2";
    path.push_back(subfigure);
    path.append(".fig");
    std::ofstream output_file(path);

    for (const auto& square : w.cube) {
        for (const auto& inner : square) {
            std::ostream_iterator<double> output_iterator(output_file, " ");
            std::copy(inner.begin(), inner.end(), output_iterator);
            output_file << "\n";
        }
        output_file << "\n";
    }
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

bool Model::load(const char &subfigure) {
    std::string path = "../saved/figure2";
    path.push_back(subfigure);
    path.append(".fig");
    if(!std::filesystem::exists(path)){
        std::cout << "Figure " << subfigure << " not found." << std::endl;
        return false;
    }
    std::cout << "path " << path << std::endl;
    std::ifstream file(path);

    std::string line;
    this->w.cube.clear();

    std::vector<std::vector<double>> square = {};
    std::vector<double> inner = {};

    while (std::getline(file, line)) {
        rtrim(line);
        std::replace(line.begin(), line.end(), '.', ',');
        size_t last = 0, next = 0;
        if (line.empty()){
            this->w.cube.push_back(square);
            square = {};
            continue;
        } else {
            inner = {};
            while ((next = line.find(DELIMITER, last)) != std::string::npos) {
                inner.push_back(std::stod(line.substr(last, next-last)));
                last = next + 1;
            }
            inner.push_back(std::stod(line.substr(last)));
            square.push_back(inner);
        }
    }
    this->w.cube.push_back(square);
    return true;
}
