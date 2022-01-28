#include <iostream>
#include "Arrays.h"
#include "Model.h"

#include "dependencies/matplotlibcpp.h"
namespace plt = matplotlibcpp;

double figsize_scale = 0.2;
// TODO Figure out how to set rcParams in matplotlib-cpp


// TODO Import MNIST dataset

std::vector<std::vector<std::vector<double>>> get_batch(size_t batch_size){
    // TODO Functionality dependent on MNIST implementation
}

void experiment(const char* subfigure, double sigma, double lambda_, size_t batches){
    // TODO Random - set seed
    Model model(sigma, lambda_);
    size_t batch_size = 1000;

    for (size_t i = 0; i < batches; i++){
        for (const auto& x : get_batch(batch_size)){
            model.update(x);
        }
        std::cout << "Completed batch " << i+1  << std::endl;
    }

    // TODO Implement Save & Load functionality for a model's W
    // np.save(f"figure2{subfigure}.npy", model.W) // python version
}

plt::Plot figure(const CubeArray& images){  // TODO Change return type -> return (fig, axes)
    const int nrows = (int) std::sqrt(images.w.size()), ncols = (int) std::sqrt(images.w.size());
    int imsize = images.w[0].size();
    std::vector<float> z(imsize * imsize);
    const int colors = 1;

    for(size_t i = 0; i < images.w.size(); i++){
        images[i].flat(z);
        const float* zptr = &(z[0]);
        for(auto value : z){
            std::cout << value << std::endl;
        }
        plt::imshow(zptr, imsize, imsize, colors);  // array, nrows, ncols, color space
        plt::show();
    }
    return plt::Plot();
}

void save_all(){
    std::vector<char> subfigures;
    subfigures = {'a', 'b', 'c', 'd'};
    for (char fig : subfigures){
        CubeArray testArray(false, 16, 5, 5);
        figure(testArray);
    }
}


int main() {
    const double learning_rate = .1;
    save_all();
    return 0;
}
