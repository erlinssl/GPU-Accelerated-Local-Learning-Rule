#include <filesystem>
#include <iostream>
#include <fstream>
#include <chrono>
#include <utility>
#include <random>

#include "Arrays.h"
#include "Model.h"
#include "dependencies/matplotlibcpp.h"

namespace plt = matplotlibcpp;

double figsize_scale = 0.2;
// TODO Figure out how to set rcParams in matplotlib-cpp


const static int GRID_SIZE = 4;
const static int RESOLUTION = 5;
const static int BATCH_SIZE = 1000;


af::array get_data() {
    std::cout << "getting data" << std::endl;
    if (std::filesystem::exists("trainingdata")) {
        std::cout << "found training data" << std::endl;

        std::ifstream f("trainingdata", std::ios::binary | std::ios::in);
        // ignore until image data
        f.ignore(16);

        std::vector<double> multi_pic_array;
        multi_pic_array.reserve(60000 * 28 * 28);
        char b;
        for (int i = 0; i < 60000 * 28 *  28; ++i) {
            f.get(b);
            multi_pic_array.emplace_back(((double) b) / 255.0);
        }
      return {60000, 28, 28, &multi_pic_array[0]};
    }
    else {
        std::cerr << "could not find training data, downloading not yet implemented" << std::endl;
        exit(1);
    }
}

auto data = get_data();

template <typename T>
CubeArray<T> get_batch(size_t batch_size){
    std::vector<std::vector<size_t>> batch_indices(batch_size, std::vector<size_t>(3));
    for(int i = 0; i < batch_size; ++i) {
        std::vector<size_t> temp;
        batch_indices[i][0] = ((int)((get_rand() * 60000.)));
        // todo hardcoded shapes
        batch_indices[i][1] = ((int)((2 + get_rand() * (28 - 4))));
        batch_indices[i][2] = ((int)((2 + get_rand() * (28 - 4))));
    }

    std::vector<std::vector<std::vector<T>>> batch;

    for (int i = 0; i < batch_indices.size(); ++i) {
        auto dt = data[batch_indices[i][0]];
        batch.emplace_back( dt.get_slices(batch_indices[i][1] - 2, batch_indices[i][1] + 3, batch_indices[i][2] - 2, batch_indices[i][2] + 3));
    }

    return CubeArray<T>(batch);
}


template <typename T>
void experiment(const char subfigure, double sigma, double lambda_, size_t nbatches){
    // TODO Set random seed for consistent experiments
    auto start = std::chrono::high_resolution_clock::now();
    Model<T> model(sigma, lambda_, GRID_SIZE, RESOLUTION);

    for (size_t i = 0; i < nbatches; i++){
        auto start = std::chrono::high_resolution_clock::now();
        af::array batch = get_batch<double>(BATCH_SIZE);
        for (size_t j = 0; j < BATCH_SIZE; j++){
            model.update(batch(j));
        }
        auto stop = std::chrono::high_resolution_clock::now();
        std::cout << "CO3: Completed batch " << i+1 << " @ " << BATCH_SIZE << " after " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()
        << "ms" << std::endl;
    }
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << "Experiment " << subfigure <<" ended after " <<
              std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms" << std::endl;
    model.save(subfigure);
}

template <typename T>
void figure(const Model<T>& model){
    std::vector<float> z(model.resolution * model.resolution, 0.0);
    const int nrows = (int) std::sqrt(model.filters), ncols = (int) std::sqrt(model.filters);
    const float* zptr = &(z[0]);
    std::vector<int> ticks = {};
    const int colors = 1;

    for(int row = 0; row < nrows; row++){
        for(int col = 0; col < ncols; col++){
            size_t index = row * nrows + col;
            model.mu[index].flat(z);

            plt::subplot2grid(nrows, ncols, row, col, 1, 1);
            plt::imshow(zptr, model.resolution, model.resolution, colors);
            plt::xticks(ticks);
            plt::yticks(ticks);
            plt::plot();
        }
    }
}

void test_batch(){
    std::cout << "Testing batch" << std::endl;
    Model<double> model(1.0, 0.5, GRID_SIZE, RESOLUTION);
    model.mu = get_batch<double>(16);
    std::cout << "Plotting batch" << std::endl;
    plt::Plot plot("test_plot");
    figure(model);
    plt::show();
}

template <typename T>
void save_all(const std::vector<char>& figs){
    plt::Plot plot("sub_fig");

    Model<T> model(1.0, 0.5, GRID_SIZE, RESOLUTION);

    for (char fig : figs){
        std::cout << "Graphing fig " << fig << std::endl;
        model.load(fig);
        figure(model);
        plt::show();
        /*
         std::string path = "../saved/figure2"
         path.emplace_back(fig);
         path.append(".pgf");
         plt::save(path);
        */
    }
}

int main() {
    const double learning_rate = .1;
    af_print(data);
    af::array randcplx = af::randu(2, 1, f64);
    std::cout << "type: " << data.type() << std::endl;

    /////// TESTING
    // test_batch();

    /////// EXPERIMENTS
    if (true){
    experiment<double>('a', 1.0, 0.5, 1000);
    save_all<double>({'a'});
    /*
    experiment<double>('b', 1.0, 0.5, 10000);
    experiment<double>('c', 0.5, 0.5, 1000);
    experiment<double>('d', 1.0, 1.0/9.0, 1000);
    save_all<double>({'a' , 'b', 'c', 'd'});
     */
    } else {
        // for testing
        experiment<double>('z', 1.0, 0.5, 100);
        save_all<double>({'z'});
    }

    Py_Finalize();
    return 0;
}