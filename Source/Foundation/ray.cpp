#include "ray.h"


Ray::Ray(const Point3D& origin, const Vector3D& direction, const double index, const bool v_pol)
    :origin{origin}, direction{unit(direction)}, refrac_index{index}, vertical_polarization{v_pol} {}

Point3D Ray::at(const double time) const {
    if (time < 0) {
        throw std::invalid_argument("`time` parameter is less than 0.\n");
    }
    return origin + direction * time;
}

std::ostream& operator<<(std::ostream& os, const Ray& ray) {
    return os << "Ray(origin = " << ray.origin << ", direction = " << ray.direction << ")\n";
}