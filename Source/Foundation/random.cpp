#include "random.h"
#include "_pcg_random.hpp"
#include <random>

double random_double() {
    static pcg_extras::seed_seq_from<std::random_device> seed_source;
    static pcg64_fast engine(seed_source);
    static std::uniform_real_distribution<double> distribution{0.0, 1.0};
    return distribution(engine);
}

double random_double(const int min, const int max) {
    return min + (max - min) * random_double();
}

Vector3D random_unit_vector() {
    while (true) {
        Vector3D vec{random_double(-1, 1), random_double(-1, 1), random_double(-1, 1)};
        if (length(vec) >= 1) { // Unbias sampling.
            continue;
        }
        else {
            return vec;
        }
    }
}

Vector3D random_hemisphere(const Vector3D& normal) {
    Vector3D vec = random_unit_vector();
    if (dot(vec, normal) > 0.0) { // In the same hemisphere as the normal.
        return vec;
    }
    else {
        return -vec;
    }
}