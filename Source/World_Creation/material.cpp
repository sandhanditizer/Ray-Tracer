#include "material.h"
#include "random.h"
#include "hit.h"
#include "color.h"
#include "shapes.h"
#include "vector-point.h"
#include <optional>
#include <cmath>


Material::Material(const std::string name, const bool emitting, Texture* texture)
    :name{name}, emitting{emitting}, texture{texture} {}

Material::~Material() {}

Vector3D reflect(const Vector3D& vector, const Vector3D& normal) {
    return {vector - 2 * dot(vector, normal) * normal};
}

Vector3D refract(const Vector3D& vector, const Vector3D& normal, const double index_ratio) {
    double cos_theta{-dot(vector, normal)};
    double discriminant{1.0 - std::pow(index_ratio, 2) * (1 - std::pow(cos_theta, 2))};
    if (discriminant >= 0) {
        return (index_ratio * vector) + (index_ratio * cos_theta - std::sqrt(discriminant)) * normal;
    }
    else { // Total internal reflection.
        return reflect(vector, normal);
    }
}

double schlick_probability(const double cos_theta, const double index_ratio) {
    double r_0 = std::pow((1 - index_ratio) / (1 + index_ratio), 2);
    return r_0 + (1 - r_0) * std::pow(1 - cos_theta, 5);
}

// -------------------------------------------------------------------

Diffuse::Diffuse(const bool emitting, Texture* texture)
    :Material{"Diffuse", emitting, texture} {}

Ray Diffuse::scatter(Ray ray, Hit hit) const {
    return {hit.position, random_hemisphere(hit.surface_normal), 1, ray.vertical_polarization};
}

// -------------------------------------------------------------------

Specular::Specular(const bool emitting, Texture* texture)
    :Material{"Specular", emitting, texture} {}

Ray Specular::scatter(Ray ray, Hit hit) const {
    Vector3D reflected_vec{reflect(ray.direction, hit.surface_normal)};
    return {hit.position, reflected_vec, 1, ray.vertical_polarization};
}

// -------------------------------------------------------------------

Metal::Metal(const bool emitting, const double fuzz, Texture* texture)
    :Material{"Metal", emitting, texture}, fuzz{fuzz} {
        if (fuzz < 0) {
            throw std::invalid_argument("Fuzz argument is less than 0.\n");
        }
    }

Ray Metal::scatter(Ray ray, Hit hit) const {
    Vector3D reflected_vec{reflect(ray.direction, hit.surface_normal)};
    return Ray{hit.position, reflected_vec + fuzz * random_unit_vector(), 1, ray.vertical_polarization};
}

// -------------------------------------------------------------------

Glass::Glass(const bool emitting, const double index, Texture* texture)
    :Material{"Glass", emitting, texture}, refrac_index{index} {}

Ray Glass::scatter(Ray ray, Hit hit) const {
    auto normal{hit.surface_normal};

    if (dot(ray.direction, normal) > 0) {
        normal *= -1;
    }
    
    double cos_theta{-dot(ray.direction, normal)};
    double probability{schlick_probability(cos_theta, ray.refrac_index/refrac_index)};

    if (random_double() < probability) {
        Vector3D reflected{reflect(ray.direction, normal)};
        return Ray{hit.position, reflected, 1, ray.vertical_polarization}; // Back into air.
    }
    else {
        Vector3D refracted{refract(ray.direction, normal, ray.refrac_index/refrac_index)};
        return Ray{hit.position, refracted, refrac_index, ray.vertical_polarization};
    }
}

// -------------------------------------------------------------------

Birefringence::Birefringence(const bool emitting, const double fai, const double sai, Texture* texture)
    :Material{"Birefringence", emitting, texture}, fast_axis_index{fai}, slow_axis_index{sai} {}

Ray Birefringence::scatter(Ray ray, Hit hit) const {
    auto normal{hit.surface_normal};

    if (dot(ray.direction, normal) > 0) {
        normal *= -1;
    }

    double cos_theta{-dot(ray.direction, normal)};
    double probability{schlick_probability(cos_theta, ray.refrac_index/fast_axis_index + ray.refrac_index/slow_axis_index)};

    if (random_double() < probability) {
        Vector3D reflected{reflect(ray.direction, normal)};
        return Ray{hit.position, reflected, 1, ray.vertical_polarization}; // Back into air.
    }
    else {
        // Given a particular polarizaiton of the ray, it will do different things in the material.
        Vector3D refracted;
        if (ray.vertical_polarization) {
            refracted = {refract(ray.direction, normal, ray.refrac_index/fast_axis_index)};
            return Ray{hit.position, refracted, fast_axis_index, !ray.vertical_polarization};
        }
        else {
            refracted = {refract(ray.direction, normal, ray.refrac_index/slow_axis_index)};
            return Ray{hit.position, refracted, slow_axis_index, !ray.vertical_polarization};
        }
    }
}