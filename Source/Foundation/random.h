#pragma once

#include "vector-point.h"


double random_double(); // [0.0, 1.0)
double random_double(const int min, const int max); // [min, max)

Vector3D random_unit_vector();
Vector3D random_hemisphere(const Vector3D& normal);