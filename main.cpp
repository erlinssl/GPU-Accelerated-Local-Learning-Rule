#include <filesystem>
#include <iostream>
#include <chrono>
#include <random>
#include <boost/compute/system.hpp>

#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/algorithm/find_if.hpp>

#include "Arrays.h"
#include "Model.h"
#include "dependencies/matplotlibcpp.h"

namespace plt = matplotlibcpp;

double learning_rate = 0.1;
size_t GRID_SIZE = 4;
size_t BATCH_SIZE = 1000;
size_t RESOLUTION = 5;

namespace compute = boost::compute;
namespace po = boost::program_options;


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

static std::vector<long> ex_times;

template <typename T>
void experiment(const char subfigure, double sigma, double lambda_, size_t nbatches){
    // TODO Set random seed for consistent experiments
    auto start = std::chrono::steady_clock::now();

    Model<T> model(sigma, lambda_, GRID_SIZE, RESOLUTION, BATCH_SIZE, learning_rate);


    compute::vector<double> gpu_data(data.cube.size(), model.context);
    compute::vector<double> rands(nbatches * BATCH_SIZE * 3, model.context);

    std::vector<double> rand_host(nbatches * BATCH_SIZE * 3);
    std::generate(rand_host.begin(), rand_host.end(), get_rand);

    compute::copy(data.cube.begin(), data.cube.end(), gpu_data.begin(), model.queue);
    compute::copy(rand_host.begin(), rand_host.end(), rands.begin(), model.queue);

    auto kernel2 = compute::kernel(model.program, "INDICES");

    kernel2.set_arg(0, rands.get_buffer());
    clSetKernelArg(kernel2, 2, BATCH_SIZE * 3 * sizeof(int), nullptr);
    kernel2.set_arg(3, gpu_data.get_buffer());
    kernel2.set_arg(4, model.batch_data.get_buffer());

    for (size_t i = 0; i < nbatches; i++){
        auto start = std::chrono::high_resolution_clock::now();
        kernel2.set_arg(1, (int)i);

        model.queue.enqueue_1d_range_kernel(kernel2, 0, BATCH_SIZE,0);

        for (size_t j = 0; j < BATCH_SIZE; j++){
            model.update(j);
        }
        model.queue.finish();
        auto stop = std::chrono::high_resolution_clock::now();
        std::cout << subfigure << "-" << "CO3: Completed batch " << i+1 << " @ " << BATCH_SIZE << " after " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()
        << "ms" << std::endl;
    }

    compute::copy(model.mugpu.begin(), model.mugpu.end(), model.results.begin(), model.queue);

    auto stop = std::chrono::steady_clock::now();
    std::clog <<
              std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "," << model.sigma << ","
              << model.lambda <<  "," << model.filters << "," << model.resolution <<  "," << BATCH_SIZE << "," << nbatches;
    std::cout << "Experiment " << subfigure <<" ended after " <<
              std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms" << std::endl;
    ex_times.push_back((stop - start).count());
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
            for (int i = 0; i < z.size(); ++i) {
                z[i] = model.results[index * model.resolution * model.resolution + i];
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
void save_all(const std::vector<char>& figs){
    plt::Plot plot("sub_fig");

    Model<T> model(1.0, 0.5, GRID_SIZE, RESOLUTION, BATCH_SIZE);

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
    }

    experiment<double>('z', sigma, lambda, nbatches);
    save_all<double>({'z'});

    Py_Finalize();
    return 0;
}
