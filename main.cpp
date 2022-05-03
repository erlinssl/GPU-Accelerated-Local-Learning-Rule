#include <filesystem>
#include <iostream>
#include <chrono>
#include <random>

#include "Arrays.h"
#include "Model.h"

#include "dependencies/matplotlibcpp.h"
#include "cifar-10/include/cifar/cifar10_reader.hpp"

namespace plt = matplotlibcpp;

static double LEARNING_RATE = .1;
static int GRID_SIZE = 4;
static int RESOLUTION = 5;
static int RES_LOWER = 2;
static int RES_UPPER = 3;
static int COLORS = 3;
static int BATCH_SIZE = 1000;


af::array get_data() {
    std::cout << "getting data" << std::endl;
    if (std::filesystem::exists("trainingdata")) {
        std::cout << "found training data" << std::endl;
        /*
         *         std::vector<double> multi_pic_array;
         *         multi_pic_array.reserve(60000 * 28 * 28);
         *         char b;
         *         for (int i = 0; i < 60000 * 28 *  28; ++i) {
         *              f.get(b);
         *              multi_pic_array.emplace_back(((double) ((unsigned char) b)) / 255.0);
         *         }
         *         return {28, 28, 60000, &multi_pic_array[0]};
         */
        std::vector<double> temp;
        temp.reserve(153600000);
        //TODO Read order ?
        for(const auto &image : cifar::read_dataset<std::vector, std::vector, uint8_t, uint8_t>().training_images) {
            /*for(int p = 0; p < 32*32; p++) {
                for(int c = 0; c < 3; c++){
                    temp.emplace_back(image[c*1024+p]);
                }
            }*/
            for(auto pixel : image) {
                temp.emplace_back(pixel);
            }
        }
        std::cout << "processed data: " << temp.size() << " items" << std::endl;
        return {32, 32, 3, 50000, &temp[0]};
    }
    else {
        std::cerr << "could not find training data, downloading not yet implemented" << std::endl;
        exit(1);
    }
}

af::array data = get_data();

template <typename T>
af::array get_batch(size_t batch_size){
    std::vector<std::vector<size_t>> batch_indices(batch_size, std::vector<size_t>(3));

    for(int i = 0; i < batch_size; ++i) {
        batch_indices[i][0] = ((unsigned long)((get_rand() * 50000.)));
        batch_indices[i][1] = ((unsigned long)((RES_LOWER + get_rand() * (32 - 2*RES_LOWER))));
        batch_indices[i][2] = ((unsigned long)((RES_LOWER + get_rand() * (32 - 2*RES_LOWER))));
    }

    std::vector<std::vector<std::vector<T>>> batch;
    af::array A = af::constant(0, RESOLUTION, RESOLUTION, COLORS, batch_indices.size());
    for (int i = 0; i < batch_indices.size(); ++i) {
        A(af::span, af::span, af::span, i) = data(af::seq(batch_indices[i][1] - RES_LOWER, batch_indices[i][1] + (RES_UPPER-1)), af::seq(batch_indices[i][2] - RES_LOWER, batch_indices[i][2] + (RES_UPPER-1)), af::span, batch_indices[i][0]);
    }
    std::cout << A.dims() << std::endl;
    return A;
}

template <typename T>
void experiment(const char subfigure, double sigma, double lambda_, size_t nbatches){
    auto start = std::chrono::high_resolution_clock::now();
    Model<T> model(sigma, lambda_, GRID_SIZE, RESOLUTION, COLORS, LEARNING_RATE);

    for (size_t i = 0; i < nbatches; i++){
        auto start = std::chrono::high_resolution_clock::now();
        af::array batch = get_batch<double>(BATCH_SIZE);
        for (int j = 0; j < BATCH_SIZE; j++){
            model.update(batch(af::span, af::span, af::span, j));
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
    std::vector<float> z(model.resolution * model.resolution * model.colors, 0.0);
    const int nrows = (int) std::sqrt(model.filters), ncols = (int) std::sqrt(model.filters);
    const float* zptr = &(z[0]);
    std::vector<int> ticks = {};
    // af_print(model.mu);

    for(int row = 0; row < nrows; row++){
        for(int col = 0; col < ncols; col++){
            size_t index = row * nrows + col;

            af::array af_z = model.mu(af::span, af::span, af::span, index);
            af_z = af_z - af::min(af::min(af::min(af_z))).scalar<T>();
            af_z = af_z/af::max(af::max(af::max(af_z))).scalar<T>();

            /*for(int i = 0; i < af_z.elements(); i++) {
                z[i] = (float) af_z(i).scalar<float>();
            }*/

            for(int i = 0; i < model.resolution; i++) {
                for(int j = 0; j < model.resolution; j++) {
                    for (int c = 0; c < model.colors; c++) {            // z layout
                        z.push_back((af_z(i, j, c).scalar<float>()));   // R_1, G_1, B_1, R_2, G_2, B_2, R_3, ...
                    }                                                   // imshow order
                }                                                       // P_1  P_3
            }                                                           // P_2  P_4

            plt::subplot2grid(nrows, ncols, row, col, 1, 1);
            plt::imshow(zptr, model.resolution, model.resolution, 3);
            plt::xticks(ticks);
            plt::yticks(ticks);
            plt::plot();
        }
    }
}

template <typename T>
void test_image(int _res_) {
    // TODO Test different row/column majors and RGB orders for imshow
    std::cout << "Testing image" << std::endl;
    RESOLUTION = _res_;
    RES_LOWER = std::floor(_res_/2);
    RES_UPPER = RESOLUTION - RES_LOWER;
    Model<T> model(1.0, 0.5, 1, RESOLUTION, COLORS, LEARNING_RATE);

    af::array image = get_batch<T>(1);
    std::vector<float> z;
    z.reserve(model.resolution * model.resolution * model.colors);

    image = image - af::min(af::min(af::min(image))).scalar<T>();
    image = image/af::max(af::max(af::max(image))).scalar<T>();

    std::cout << "image dims: " << image.dims() << std::endl;
    // TODO colors?
    for(int c = 0; c < 3; c++){
        for(int i = 0; i < RESOLUTION; i++) {
            for (int j = 0; j < RESOLUTION; j++) {
                auto temp = (float) image(i, j, c).scalar<float>();
                z.emplace_back(temp);
            }
        }
    }
    /*
    for(int i = 0; i < image.elements(); i++) {
        z[i] = (float) image(i).scalar<float>();
    }
    */

    for(auto item : z){
        if(item > 255) {
            std::cout << item << std::endl;
        }
        if(item < 0) {
            std::cout << item << std::endl;
        }
    }
    std::cout << std::endl;
    std::cout << z.size() << std::endl;

    const float* zptr = &(z[0]);
    plt::imshow(zptr, RESOLUTION, RESOLUTION, 3);
    plt::show();
}

template <typename T>
void test_batch(int _res_){
    std::cout << "Testing batch" << std::endl;
    RESOLUTION = _res_;
    RES_LOWER = std::floor(_res_/2);
    RES_UPPER = RESOLUTION - RES_LOWER;

    Model<T> model(1.0, 0.5, GRID_SIZE, RESOLUTION, COLORS, LEARNING_RATE);
    model.mu = get_batch<T>(GRID_SIZE*GRID_SIZE);
    std::cout << "Plotting batch" << std::endl;
    plt::Plot plot("test_plot");
    figure(model);
    plt::show();
}

template <typename T>
void save_all(const std::vector<char>& figs){
    plt::Plot plot("sub_fig");

    Model<T> model(1.0, 0.5, GRID_SIZE, RESOLUTION, COLORS);

    for (char fig : figs){
        if(model.load(fig)){
            std::cout << "Loaded figure " << fig << std::endl;
            figure(model);
            plt::show();
        }
    }
}

int main(int argc, char* argv[]) {
    /////// EXPERIMENTS
    // TODO af seeds not necessarily consistent with other versions of program, \
        consider use random() and inserting into arrays, might not be necessary
    af::setSeed(1234);

    if (argc < 7){
        experiment<double>('a', 1.0, 0.5, 100);
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
        COLORS = std::stoi(argv[8]);

        RES_LOWER = std::floor(RESOLUTION/2);
        RES_UPPER = RESOLUTION - RES_LOWER;

        test_image<float>(RESOLUTION);
        // experiment<float>('z', sigma, lambda, nbatches);
        // save_all<float>({'z'});
    }

    Py_Finalize();
    return 0;
}