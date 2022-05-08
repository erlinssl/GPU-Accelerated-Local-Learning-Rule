#include <random>

static std::random_device rd;
static std::mt19937 mt(rd());
static std::uniform_real_distribution<double> dist(0.0, 1.0);

static double get_rand() {
    return dist(mt);
}
