#include "world.h"
#include "constants.h"
#include "material.h"


void World::add(const Sphere& sphere) {
        objects.push_back(std::make_shared<Sphere>(sphere.center, sphere.radius, sphere.material));
}

void World::add(const Triangle& triangle) {
        objects.push_back(std::make_shared<Triangle>(triangle.vertex0, triangle.vertex1, triangle.vertex2, triangle.material));
}

std::optional<Hit> World::find_nearest(const Ray& ray) const {
    double time{Constants::Infinity};
    std::shared_ptr<Shape> nearest_shape{nullptr};

    for (auto object : objects) {
        std::optional<double> t = object->intersect(ray);
        if (t.has_value() && t.value() < time) {
            nearest_shape = object;
            time = t.value();
        }
    }
    
    if (nearest_shape) {
        return nearest_shape->construct_hit(ray, time);
    }
    else {
        return std::nullopt;
    }
}

Color trace_path(const World& world, const Ray& ray, const int depth) {
    if (depth < 0) {
        throw std::invalid_argument("Recursive depth is less than 0.\n");
    }

    if (depth == 0) {
        return Black;
    }

    std::optional<Hit> hit = world.find_nearest(ray);
    if (!hit.has_value()) {
        return DeepOcean * 0.6;
    }

    std::pair<double, double> coordinates = hit->shape->UV_coordinates(hit.value());
    Material* material = hit->shape->material;
        
    if (material->emitting) {
        Color color = material->texture->UV(coordinates.first, coordinates.second);
        return color * std::pow(std::abs(dot(hit->surface_normal, ray.direction)), 0.333);
    }
    Ray scattered = material->scatter(ray, hit.value());
    return trace_path(world, scattered, depth - 1) * material->texture->UV(coordinates.first, coordinates.second);
}