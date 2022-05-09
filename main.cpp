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


static double learning_rate = 0.1;
static int GRID_SIZE = 4;
static int RESOLUTION = 5;
static int LOWER_RES = 3;
static int UPPER_RES = 2;
static int BATCH_SIZE = 1000;

/*
 * Reads the MNIST dataset from binary file located at "./data/train-images-idx3-ubyte"
 * @return an array filled with the pixel data of handwritten numbers
 */
CubeArray<double> get_data() {
    std::cout << "getting data" << std::endl;

    if (std::filesystem::exists("trainingdata")) {
        std::cout << "found training data" << std::endl;

        std::ifstream f("trainingdata", std::ios::binary | std::ios::in);
        // ignore until image data
        f.ignore(16);

        std::vector<std::vector <std::vector<double>>> multi_pic_vector;
        for(int i = 0; i < 60000; ++i) {
            std::vector<std::vector<double>> single_pic_vector;
            single_pic_vector.reserve(28);
            for(int j = 0; j < 28; ++j) {
                single_pic_vector.emplace_back(std::vector<double>());
            }
            multi_pic_vector.emplace_back(single_pic_vector);
        }

        char b;
        unsigned int rowCounter = 0;
        unsigned int picCounter = 0;
        unsigned int columnCounter = 0;
        while (f.get(b)) {
            auto c = (unsigned char) b;
            multi_pic_vector[picCounter][rowCounter].emplace_back(((double) c) / 255.0);
            columnCounter++;
            if (columnCounter > 27) {
                rowCounter++;
                columnCounter = 0;
                if (rowCounter > 27) {
                    picCounter++;
                    rowCounter = 0;
                }
            }
        }
        std::cout << "number of pictures: " << picCounter << std::endl;
        return CubeArray<double>(multi_pic_vector);
    }
    else {
        std::cerr << "could not find training data, downloading not yet implemented" << std::endl;
        exit(1);
    }
}

auto data = get_data();

/*
 * Used to get some number of patches that each represent a random part of one of the 60000 images from the dataset
 * @param batch_size the number of patches to get
 * @return a (batch_size, RESOLUTION, RESOLUTION) array of samples/patches
 */
template <typename T>
CubeArray<T> get_batch(size_t batch_size){
    std::vector<std::vector<size_t>> batch_indices(batch_size, std::vector<size_t>(3));
    for(int i = 0; i < batch_size; ++i) {
        std::vector<size_t> temp;
        batch_indices[i][0] = ((int)((get_rand() * 60000.)));
        // todo hardcoded shapes
        batch_indices[i][1] = ((int)((LOWER_RES + get_rand() * (28 - 2*LOWER_RES))));
        batch_indices[i][2] = ((int)((LOWER_RES + get_rand() * (28 - 2*LOWER_RES))));
    }

    std::vector<std::vector<std::vector<T>>> batch;

    for (int i = 0; i < batch_indices.size(); ++i) {
        auto dt = data[batch_indices[i][0]];
        batch.emplace_back( dt.get_slices(batch_indices[i][1] - LOWER_RES, batch_indices[i][1] + UPPER_RES, batch_indices[i][2] - LOWER_RES, batch_indices[i][2] + UPPER_RES));
    }

    return CubeArray<T>(batch);
}

static std::vector<long> ex_times;

/*
 * The main method used for finding filters
 * @param subfigure char to be used for saving/loading
 * @param nbatches number of batches to run through
 */
template <typename T>
void experiment(const char subfigure, double sigma, double lambda_, size_t nbatches){
    // TODO Set random seed for consistent experiments
    auto start = std::chrono::steady_clock::now();
    Model<T> model(sigma, lambda_, GRID_SIZE, RESOLUTION);

    for (size_t i = 0; i < nbatches; i++){
        auto start = std::chrono::high_resolution_clock::now();
        CubeArray<T> batch = get_batch<double>(BATCH_SIZE);
        for (size_t j = 0; j < BATCH_SIZE; j++){
            model.update(batch[j]);
        }
        auto stop = std::chrono::high_resolution_clock::now();
        std::cout << subfigure << "-" << "CO3: Completed batch " << i+1 << " @ " << BATCH_SIZE << " after " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()
        << "ms" << std::endl;
    }
    auto stop = std::chrono::steady_clock::now();
    std::clog <<
    std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "," << model.sigma << ","
    << model.lambda <<  "," << model.filters << "," << model.resolution <<  "," << BATCH_SIZE << "," << nbatches;
    std::cout << "Experiment " << subfigure <<" ended after " <<
              std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms" << std::endl;
    ex_times.push_back((stop - start).count());
    model.save(subfigure);
}

/*
 * Method used to plot a model's mu
 */
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
            model.w[index].flat(z);

            plt::subplot2grid(nrows, ncols, row, col, 1, 1);
            plt::imshow(zptr, model.resolution, model.resolution, colors);
            plt::xticks(ticks);
            plt::yticks(ticks);
            plt::plot();
        }
    }
}

/*
 * Gets a number of images equal to the amount of filters being used and displays them.
 * Useful for finding out if dataset was properly read
 */
void test_batch(){
    std::cout << "Testing batch" << std::endl;
    Model<double> model(1.0, 0.5, GRID_SIZE, RESOLUTION);
    model.w = get_batch<double>(16);
    std::cout << "Plotting batch" << std::endl;
    plt::Plot plot("test_plot");
    figure(model);
    plt::show();
}

/*
 * Loads a model with previously found filters and then calls figure to show them graphically.
 * Originally used to save .pgf files, thus the name save_all
 * @param figs f.ex. {'a', 'b', 'c'}, depending on which subfigs to be loaded
 */
template <typename T>
void save_all(const std::vector<char>& figs){
    plt::Plot plot("sub_fig");

    Model<T> model(1.0, 0.5, GRID_SIZE, RESOLUTION);

    for (char fig : figs){
        std::cout << "Graphing fig " << fig << std::endl;
        model.load(fig);
        figure(model);
        plt::show();
    }
}

int main(int argc, char* argv[]) {
    double sigma = 1.0;
    double lambda = 0.5;
    int nbatches = 1000;

    if (argc > 7) {
        sigma = std::stod(argv[1]);
        lambda = std::stod(argv[2]);
        nbatches = std::stoi(argv[3]);
        GRID_SIZE = std::stoi(argv[4]);
        BATCH_SIZE = std::stoi(argv[5]);
        RESOLUTION = std::stoi(argv[6]);
        learning_rate = std::stod(argv[7]);

        LOWER_RES = std::floor(RESOLUTION/2);
        UPPER_RES = RESOLUTION - LOWER_RES;
    }

    experiment<double>('a', sigma, lambda, nbatches);
    //save_all<double>({'a'});

    Py_Finalize();
    return 0;
}
