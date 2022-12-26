#include "camera.h"
#include "constants.h"
#include "random.h"
#include <cmath>


Camera::Camera(const Point3D& position, const Point3D& target, const Vector3D& up, const double fov, const double aspect_ratio) 
    :position{position} {
        if (fov > 360) {
            throw std::invalid_argument("Decrease your field of view.\n");
        }
        
        Vector3D w{unit(position - target)};
        Vector3D u{unit(cross(up, w))};
        Vector3D v{unit(cross(u, w))};

        double radians{(fov * Constants::Pi) / 180};
        double width{2 * tan(radians / 2.0)};
        double height{width / aspect_ratio};
        
        Point3D origin{position - w};

        horizontal = u * width;
        vertical = v * height;
        upper_left_corner = origin - (horizontal / 2.0) - (vertical/ 2.0);
    }

Ray Camera::compute_ray(const double s, const double t) const {
    return Ray(position, upper_left_corner + (s * horizontal) + (t * vertical) - position, 1, random_polarization());
}

bool random_polarization() {
    double x = random_double(0, 2);
    if (x < 1) {
        return true;
    }
    else {
        return false;
    }
}