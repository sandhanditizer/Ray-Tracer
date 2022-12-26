#pragma once

#include "hit.h"
#include "shapes.h"
#include "color.h"
#include <vector>
#include <memory>


class Ray;

class World {
public:
    void add(const Sphere& sphere);
    void add(const Triangle& triangle);
    std::optional<Hit> find_nearest(const Ray& ray) const;

    std::vector<std::shared_ptr<Shape>> objects;
};

Color trace_path(const World& world, const Ray& ray, const int depth);