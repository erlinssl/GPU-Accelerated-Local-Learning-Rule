#define WITHOUT_NUMPY
#include <filesystem>
#include <iostream>
#include <fstream>
#include <iterator>

#include "Arrays.h"
#include "Model.h"

#include "dependencies/matplotlibcpp.h"

namespace plt = matplotlibcpp;

double figsize_scale = 0.2;
// TODO Figure out how to set rcParams in matplotlib-cpp


std::vector<std::vector <std::vector<double>>> get_data() {
    std::cout << "getting data" << std::endl;

    if (std::filesystem::exists("trainingdata")) {
        std::cout << "found training data" << std::endl;

        std::ifstream f("trainingdata", std::ios::binary | std::ios::in);
        // ignore until image data
        //f.ignore(16);

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
            multi_pic_vector[picCounter][rowCounter].emplace_back(((double) b) / 255.0);
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
        return multi_pic_vector;
    }
    else {
        std::cerr << "could not find training data, downloading not yet implemented" << std::endl;
        exit(1);
    }
}


CubeArray get_batch(size_t batch_size){
    // TODO Functionality dependent on MNIST implementation
}

/*
void experiment(const char* subfigure, double sigma, double lambda_, size_t nbatches){
    // TODO Random - set seed
    Model model(sigma, lambda_);
    size_t batch_size = 1000;

    for (size_t i = 0; i < nbatches; i++){
        CubeArray batch = get_batch(batch_size);
        for (size_t j = 0; j < batch.size(); j++){
            model.update(batch[j]);
        }
        std::cout << "Completed batch " << i+1  << std::endl;
    }

    // TODO Implement Save & Load functionality for a model's W
    // np.save(f"figure2{subfigure}.npy", model.W) // python version
}

plt::Plot figure(const CubeArray& images){
    plt::Plot plot("grid");
    const int nrows = (int) std::sqrt(images.w.size()), ncols = (int) std::sqrt(images.w.size());
    int imsize = (int) images.w[0].size();
    std::vector<float> z(imsize * imsize);
    const float* zptr = &(z[0]);
    std::vector<int> ticks = {};
    const int colors = 1;

    for(int row = 0; row < nrows; row++){
        for(int col = 0; col < ncols; col++){
            size_t index = row * nrows + col;
            images[index].flat(z);

            plt::subplot2grid(nrows, ncols, row, col, 1, 1);
            plt::imshow(zptr, imsize, imsize, colors);
            plt::xticks(ticks);
            plt::yticks(ticks);
            plt::plot();
        }
    }
    return plot;
}


void save_all(){
    std::vector<char> subfigures = {'a', 'b', 'c', 'd'};
    plt::Plot plot("sub_fig");
    for (char fig : subfigures){
        CubeArray testArray(false, 16, 5, 5);
        plot = figure(testArray);
        plt::show();
    }
}
 */

int main() {
    const double learning_rate = .1;

    // experiment("a", 1.0, 0.5, 1000);  // experiment(subfigure="a", sigma=1.0, lambda_=0.5, batches=1000)
    // experiment("b", 1.0, 0.5, 10000);  // experiment(subfigure="b", sigma=1.0, lambda_=0.5, batches=10000)
    // experiment("c", 0.5, 0.5, 1000);  // experiment(subfigure="c", sigma=0.5, lambda_=0.5, batches=1000)
    // experiment("d", 1.0, 1.0/9.0, 1000);  // experiment(subfigure="d", sigma=1.0, lambda_=1.0/9.0, batches=1000)

    // save_all();
    get_data();
    return 0;
}