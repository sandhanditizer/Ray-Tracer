#pragma once

#include "vector-point.h"


class Ray {
public:
    Ray(const Point3D& origin, const Vector3D& direction, const double index, const bool polarization);
    Point3D at(const double time) const;
  
    Point3D origin;
    Vector3D direction;
    double refrac_index;
    bool vertical_polarization;
};

std::ostream& operator<<(std::ostream& os, const Ray& ray);