#pragma once

#include "vector-point.h"
#include "ray.h"


class Camera {
public:
    Camera(const Point3D& position, const Point3D& target, const Vector3D& up, const double fov, const double aspect_ratio);

    // `s` and `t` are normalized values [0, 1] that describe the position along the horizontal 
    // and vertical directions in the camera's coordinate system.
    Ray compute_ray(const double s, const double t) const;

private:
    Point3D position;
    Point3D upper_left_corner;
    Vector3D horizontal, vertical; // Vectors along u and v with lengths width and height.
};

bool random_polarization(); // Determines the polarity of a particular ray.