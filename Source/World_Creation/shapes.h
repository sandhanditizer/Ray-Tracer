# pragma once

#include "vector-point.h"
#include "hit.h"
#include "material.h"
#include <optional>
#include <memory>


class Ray;

class Shape {
public:
    Shape(const std::string& shape_name, Material* material);
    virtual ~Shape() = 0;
    virtual std::optional<double> intersect(const Ray& ray) const = 0;
    virtual Hit construct_hit(const Ray& ray, const double time) const = 0;
    virtual std::pair<double, double> UV_coordinates(const Hit& hit) const = 0;

    std::string shape_name;
    Material* material = nullptr;
};

class Sphere : public Shape {
public:
    Sphere(const Point3D& center, const double radius, Material* material);

    std::optional<double> intersect(const Ray& ray) const override;
    Hit construct_hit(const Ray& ray, const double time) const override;
    std::pair<double, double> UV_coordinates(const Hit& hit) const override;

    Point3D center;
    double radius;
};

class Triangle : public Shape {
public:
    Triangle(const Point3D& v_0, const Point3D& v_1, const Point3D& v_2, Material* material);

    std::optional<double> intersect(const Ray& ray) const override;
    Hit construct_hit(const Ray& ray, const double time) const override;
    std::pair<double, double> UV_coordinates(const Hit& hit) const override;

    Point3D vertex0, vertex1, vertex2;
    Vector3D edge1, edge2;
    Vector3D surface_normal; 
};