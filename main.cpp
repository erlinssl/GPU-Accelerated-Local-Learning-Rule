#define WITHOUT_NUMPY

#include <iostream>
#include "Arrays.h"
#include "Model.h"

#include "dependencies/matplotlibcpp.h"

double figsize_scale = 0.2;

// TODO matplotlib.cpp  AND  import MNIST dataset

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

int figure(const CubeArray images){  // TODO change return type -> return (fig, axes)
}

void save_all(){
}




int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
