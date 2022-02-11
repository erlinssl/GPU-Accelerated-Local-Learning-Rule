#include <filesystem>
#include <iostream>
#include <fstream>

#include "Arrays.h"
#include "Model.h"

#include "dependencies/matplotlibcpp.h"

namespace plt = matplotlibcpp;

double figsize_scale = 0.2;
// TODO Figure out how to set rcParams in matplotlib-cpp


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
        return CubeArray<double>(multi_pic_vector);
    }
    else {
        std::cerr << "could not find training data, downloading not yet implemented" << std::endl;
        exit(1);
    }
}

auto data = get_data();

template <typename T>
CubeArray<T> get_batch(size_t batch_size){
    // batch_indexes = np.floor(np.random.rand(batch_size, 1) * data.shape[0]).astype(int)
    // todo don't know if this needs to be nested, but it is nested in python :)
    std::vector<std::vector<double>> rand_array;
    rand_array.emplace_back(std::vector<double>());
    for (int i = 0; i < batch_size; ++i) {
        rand_array[0].emplace_back((double)rand()/(double)RAND_MAX);
    }
    SquareArray s(rand_array);
    SquareArray<double> multiplied_array = (s * 60000);
    SquareArray<int> batch_indexes(multiplied_array.size(), multiplied_array[0].size());
    for (int i = 0; i < multiplied_array.size(); ++i) {
        for (int j = 0; j < multiplied_array[i].size(); ++j) {
            batch_indexes[i].emplace_back((int) multiplied_array[i][j]);
        }
    }

    // batch_indexes = np.concatenate((batch_indexes, np.floor(2 + np.random.rand(batch_size, 1) * (data.shape[1] - 4)).astype(int)), axis=1)
    // batch_indexes = np.concatenate((batch_indexes, np.floor(2 + np.random.rand(batch_size, 1) * (data.shape[2] - 4)).astype(int)), axis=1)
    for (int i = 0; i < 2; ++i) {
        auto random_multiplied_square = (SquareArray<double>(batch_size, 1) * (28 - 4)) + 2;
        SquareArray<int> floored(random_multiplied_square.size(), random_multiplied_square[0].size());
        for (int i = 0; i < random_multiplied_square.size(); ++i) {
            for (int j = 0; j < random_multiplied_square[i].size(); ++j) {
                floored[i].emplace_back(random_multiplied_square[i][j]);
            }
        }
        batch_indexes.concatenate(floored);
    }
    // batch = np.zeros((batch_size, 5, 5))
    auto batch = CubeArray<double>(true, 1000, 5, 5);


    /*
     for count, index in enumerate(batch_indexes):
     //index 0 er det samme som å ta batch_indexes[i][0]
        batch[count] = data[index[0], index[1] - 2:index[1] + 3, index[2] - 2:index[2] + 3]
     return batch
     */
    for (int i = 0; i < batch_indexes.size(); ++i) {
        auto outer_slice = std::vector<std::vector<double>>(batch_indexes[i].begin() + batch_indexes[i][1] - 2, batch_indexes[i].begin() + batch_indexes[i][1 + 3]);
        // todo probably does not need to be nested
        auto inner_slice = std::vector<std::vector<double>>();
        inner_slice.emplace_back(std::vector<double>());
        for (int j = 0; j < outer_slice.size(); ++j) {
            for (auto k : std::vector<double>(outer_slice.begin() + batch_indexes[i][2] - 2, outer_slice.begin() + batch_indexes[i][2] + 3)) {
                inner_slice[0].emplace_back(k);
            }
        }
        batch[i] = SquareArray(inner_slice);

    }
    return batch;
}

template <typename T>
void experiment(const char subfigure, double sigma, double lambda_, size_t nbatches){
    // TODO Set random seed for consistent experiments
    Model<T> model(sigma, lambda_);

    size_t batch_size = 1000;

    for (size_t i = 0; i < nbatches; i++){
        CubeArray<T> batch = get_batch<double>(batch_size);
        for (size_t j = 0; j < batch.size(); j++){
            model.update(batch[j]);
        }
        std::cout << "Completed batch " << i+1  << std::endl;
    }

    model.save(subfigure);
}

template <typename T>
void figure(const CubeArray<T>& images){
    const int nrows = (int) std::sqrt(images.cube.size()), ncols = (int) std::sqrt(images.cube.size());
    int imsize = (int) images.cube[0].size();
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
}

template <typename T>
void save_all(){
    std::vector<char> subfigures = {'a'}; //, 'b', 'c', 'd'};
    plt::Plot plot("sub_fig");

    Model<T> model(1.0, 0.5);
    for (char fig : subfigures){
        std::cout << "Handling fig " << fig << std::endl;
        model.load(fig);
        figure(model.w);
        plt::show();
        /*
         std::string path = "../saved/figure2"
         path.push_back(fig);
         path.append(".pgf");
         plt::save(path);
        */
    }
}

int main() {
    //todo what does this do
    srand((unsigned)time(NULL));
    const double learning_rate = .1;

    experiment<double>('a', 1.0, 0.5, 1000);  // experiment(subfigure="a", sigma=1.0, lambda_=0.5, batches=1000)
    // experiment('b', 1.0, 0.5, 10000);  // experiment(subfigure="b", sigma=1.0, lambda_=0.5, batches=10000)
    experiment<double>('c', 0.5, 0.5, 1000);  // experiment(subfigure="c", sigma=0.5, lambda_=0.5, batches=1000)
    // experiment('d', 1.0, 1.0/9.0, 1000);  // experiment(subfigure="d", sigma=1.0, lambda_=1.0/9.0, batches=1000)

    save_all<double>();
    // get_data();
    std::cout << "debug" << std::endl;
    return 0;
}