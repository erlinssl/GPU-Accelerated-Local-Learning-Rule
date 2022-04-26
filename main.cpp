#include <filesystem>
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>
#include "Arrays.h"
#include "Model.h"
#include "dependencies/matplotlibcpp.h"

namespace plt = matplotlibcpp;

static double LEARNING_RATE = .1;
static int GRID_SIZE = 4;
static int RESOLUTION = 5;
static int BATCH_SIZE = 1000;


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
            multi_pic_array.emplace_back(((double) ((unsigned char) b)) / 255.0);
        }
        return {28, 28, 60000, &multi_pic_array[0]};
        // return af::reorder(x, 2, 0, 1);
    }
    else {
        std::cerr << "could not find training data, downloading not yet implemented" << std::endl;
        exit(1);
    }
}

af::array data = get_data();
template <typename T>
af::array get_batch(size_t batch_size){
    //todo convert to using batch_indices_2
    std::vector<std::vector<size_t>> batch_indices(batch_size, std::vector<size_t>(3));
    af::array batch_indices_2 = af::randu(batch_size, 3, f64);
    batch_indices_2.col(0) *= 60000;
    batch_indices_2.cols(1, 2) *= (28 - 4);

    batch_indices_2.cols(1, 2) += 2;

    for(int i = 0; i < batch_size; ++i) {
        std::vector<size_t> temp;
        batch_indices[i][0] = ((unsigned long)((get_rand() * 60000.)));
        // todo hardcoded shapes
        batch_indices[i][1] = ((unsigned long)((2 + get_rand() * (28 - 4))));
        batch_indices[i][2] = ((unsigned long)((2 + get_rand() * (28 - 4))));
    }

    std::vector<std::vector<std::vector<T>>> batch;
    af::array A = af::constant(0, 5, 5, batch_indices.size());
    for (int i = 0; i < batch_indices.size(); ++i) {
        A(af::span, af::span, i) = data(af::seq(batch_indices[i][1] - 2, batch_indices[i][1] + 2), af::seq(batch_indices[i][2] - 2, batch_indices[i][2] + 2), batch_indices[i][0]);
    }
    return A;
}

template <typename T>
void experiment(const char subfigure, double sigma, double lambda_, size_t nbatches){
    auto start = std::chrono::high_resolution_clock::now();
    Model<T> model(sigma, lambda_, GRID_SIZE, RESOLUTION, LEARNING_RATE);

    for (size_t i = 0; i < nbatches; i++){
        auto start = std::chrono::high_resolution_clock::now();
        af::array batch = get_batch<double>(BATCH_SIZE);
        for (int j = 0; j < BATCH_SIZE; j++){
            model.update(batch(af::span, af::span, j));
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

            af::array af_z = model.mu( af::span, af::span, index);

            for(int i = 0; i < af_z.elements(); i++) {
                z[i] = (float) af_z(i).scalar<T>();
            }

            plt::subplot2grid(nrows, ncols, row, col, 1, 1);
            plt::imshow(zptr, model.resolution, model.resolution, colors);
            plt::xticks(ticks);
            plt::yticks(ticks);
            plt::plot();
        }
    }
}

template <typename T>
void test_batch(){
    std::cout << "Testing batch" << std::endl;
    Model<T> model(1.0, 0.5, GRID_SIZE, RESOLUTION);
    model.mu = get_batch<T>(16);
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
        if(model.load(fig)){
            std::cout << "Loaded figure " << fig << std::endl;
            figure(model);
            plt::show();
        }
        /*
         std::string path = "../saved/figure2"
         path.emplace_back(fig);
         path.append(".pgf");
         plt::save(path);
        */
    }
}

int main(int argc, char* argv[]) {
    /////// EXPERIMENTS
    // true for original four experiments
    // false for script experiment
    // TODO af seeds not necessarily consistent with other iterations
    af::setSeed(1234);
    if (argc < 7){
        experiment<double>('a', 1.0, 0.5, 1000);
        af::setSeed(1234);
        experiment<double>('b', 1.0, 0.5, 10000);
        af::setSeed(1234);
        experiment<double>('c', 0.5, 0.5, 1000);
        af::setSeed(1234);
        experiment<double>('d', 1.0, 1.0/9.0, 1000);
        save_all<double>({'a' , 'b', 'c', 'd'});
    } else {
        double sigma = std::stod(argv[1]);
        double lambda = std::stod(argv[2]);
        int nbatches = std::stoi(argv[3]);
        GRID_SIZE = std::stoi(argv[4]);
        BATCH_SIZE = std::stoi(argv[5]);
        RESOLUTION = std::stoi(argv[6]);
        LEARNING_RATE = std::stod(argv[7]);

        experiment<float>('z', sigma, lambda, nbatches);
        save_all<float>({'z'});
    }

    Py_Finalize();
    return 0;
}