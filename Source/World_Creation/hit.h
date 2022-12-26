#pragma once

#include "vector-point.h"
#include "shapes.h"


class Shape;

class Hit {
public:
    Hit(const double time, const Point3D& position, const Vector3D& normal, const Shape* shape);
    ~Hit();

    double time;
    Point3D position;
    Vector3D surface_normal;
    const Shape* shape = nullptr;
};