#pragma once

#include "color.h"
#include "ray.h"
#include "texture.h"


class Hit;

class Material {
public:
    Material(const std::string name, const bool emitting, Texture* texture);
    virtual ~Material() = 0;
    virtual Ray scatter(Ray ray, Hit hit) const = 0;

    std::string name;
    bool emitting;
    Texture* texture = nullptr;
};

Vector3D reflect(const Vector3D& vector, const Vector3D& normal);
Vector3D refract(const Vector3D& vector, const Vector3D& normal, const double index);
double schlick_probability(const double cos_theta, const double index_ratio);

// -------------------------------------------------------------------
// Derived classes of Material.

class Diffuse : public Material {
public:
    Diffuse(const bool emitting, Texture* texture);
    Ray scatter(Ray ray, Hit hit) const override;
};

class Specular : public Material {
public:
    Specular(const bool emitting, Texture* texture);
    Ray scatter(Ray ray, Hit hit) const override;
};

class Metal : public Material {
public:
    Metal(const bool emitting, const double fuzz, Texture* texture);
    Ray scatter(Ray ray, Hit hit) const override;

private:
    double fuzz; // Radius of sphere of uncertainty in reflection direction.
};

class Glass : public Material {
public:
    Glass(const bool emitting, const double index, Texture* texture);
    Ray scatter(Ray ray, Hit hit) const override;

private:
    double refrac_index; // Index of refraction for glass.
};

class Birefringence : public Material {
public:
    Birefringence(const bool emitting, const double fai, const double sai, Texture* texture);
    Ray scatter(Ray ray, Hit hit) const override;

private:
    double fast_axis_index, slow_axis_index; // Indexs of a birefringent materials
};