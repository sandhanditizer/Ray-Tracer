#include "hit.h"
#include "constants.h"


Hit::Hit(const double time, const Point3D& position, const Vector3D& normal, const Shape* shape)
    :time{time}, position{position}, surface_normal{unit(normal)}, shape{shape} {
        double len{length(unit(normal))};
        if (len > 1 + Constants::Epsilon || len < 1 - Constants::Epsilon) {
            throw std::runtime_error("Unit normal vector has a length that is not equal to 1.\n");
        }
        if (time < 0) {
            throw std::invalid_argument("Time is less than 0.\n");
        }
    }

Hit::~Hit() {}