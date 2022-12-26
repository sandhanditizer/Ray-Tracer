#include "shapes.h"
#include "constants.h"
#include "ray.h"


Shape::Shape(const std::string& shape_name, Material* material)
    :shape_name{shape_name}, material{material} {}

Shape::~Shape() {}

// ----------------------------------------------------------------------

Sphere::Sphere(const Point3D& center, const double radius, Material* material)
    :Shape{"Sphere", material}, center{center}, radius{radius} 
    {
        if (radius <= 0) {
            throw std::invalid_argument("Radius of sphere is less than or equal to zero.\n");
        }
    }

std::optional<double> Sphere::intersect(const Ray& ray) const {
    Vector3D OC{center - ray.origin};
    double R{dot(ray.direction, OC)};
    double h2{radius*radius - dot(OC, OC) + R*R};

    if (h2 < 0) {
        return std::nullopt; // No hit
    }
    
    double h{std::sqrt(h2)};
    if ((R - h) > Constants::Epsilon) {
        return R - h; // Intersection from outside => t = R - h
    }
    else if ((R + h) > Constants::Epsilon) {
        return R + h; // Intersection from inside => t = R + h
    }
    else { 
        return std::nullopt;
    } 
}

Hit Sphere::construct_hit(const Ray& ray, const double time) const {
    if (time < 0) {
        throw std::invalid_argument("Cannot construct a hit: time is less than 0.\n");
    }

    Point3D point{ray.at(time)}; // Point of intersection

    // The ray.at(time) is not on the sphere
    double len{length(point - center)};
    if (len < (radius - Constants::Epsilon) || len > (radius + Constants::Epsilon)) {
        throw std::runtime_error("ray.at(time) is not on the sphere.\n");
    }

    Vector3D surface_normal{(point - center) / radius};
    return Hit{time, point, surface_normal, this};
}

std::pair<double, double> Sphere::UV_coordinates(const Hit& hit) const {
    double theta{acos(hit.surface_normal.z)};
    double phi{atan2(hit.surface_normal.y, hit.surface_normal.x)};
    //double u{1 + phi / Constants::Pi}; // [0, 2]
    double u{0.5 + phi / (Constants::Pi * 2)}; // [0, 1]
    double v{theta / Constants::Pi}; // [0, 1]
    return std::pair<double, double>{u, v};
}

// ----------------------------------------------------------------------

Triangle::Triangle(const Point3D& vertex0, const Point3D& vertex1, const Point3D& vertex2, Material* material)
    :Shape{"Triangle", material}, vertex0{vertex0}, vertex1{vertex1}, vertex2{vertex2}, 
    edge1{vertex1 - vertex0},
    edge2{vertex2 - vertex0} 
    {
        try {
            surface_normal = unit(cross(edge1, edge2));
        }
        catch(const std::exception& err) {
            std::cerr << err.what() << "Triangle Error\n";
        }
    }


std::optional<double> Triangle::intersect(const Ray& ray) const {
    // Moeller-Trumbore intersection
    Vector3D h{cross(ray.direction, edge2)};
    double a{dot(edge1, h)};
    if (a > -Constants::Epsilon && a < Constants::Epsilon) {
        return std::nullopt; // Too close or parallel to the triangle.
    }

    double f{1.0 / a};
    Vector3D s{ray.origin - vertex0};
    double u{f * dot(s, h)};
    if (u < 0.0 || u > 1.0) {
        return std::nullopt; // Ray is outside of u: [0, 1].
    }

    Vector3D q{cross(s, edge1)};
    double v{f * dot(ray.direction, q)};
    if (v < 0.0 || (u + v) > 1.0) {
        return std::nullopt; // Ray is outside of v: [0, 1].
    }

    double time{f * dot(edge2, q)};
    if (time > Constants::Epsilon) {
        return time;
    }
    else {
        return std::nullopt;
    }
}

Hit Triangle::construct_hit(const Ray& ray, const double time) const {
    if (time < 0) {
        throw std::runtime_error("Cannot construct a hit: time is less than 0.\n");
    }

    Point3D position{ray.at(time)};
    bool negative{std::signbit(dot(ray.direction, surface_normal))};
    if (negative) {
        return Hit{time, position, surface_normal, this};
    }
    else {
        return Hit{time, position, -surface_normal, this};
    }
}

std::pair<double, double> Triangle::UV_coordinates(const Hit& hit) const {
    Vector3D P = hit.position - vertex0;
    double u{dot(P, edge1) / length(edge1)};
    double v{dot(P, edge2) / length(edge2)};
    double mag = (u + v) / 2.0;
    return {u/mag, v/mag};
}