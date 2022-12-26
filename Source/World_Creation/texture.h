#pragma once

#include "color.h"
#include <iostream>
#include <vector>


class Texture {
public:
    Texture(const std::string& name, const Color& color);
    virtual ~Texture();
    virtual Color UV(double u, double v) const = 0;

    std::string name;
    Color color;
};

class Solid : public Texture {
public:
    Solid(const std::string& name, const Color& color);
    Color UV(double u, double v) const override;
};

class Checkerboard : public Texture {
public:
    Checkerboard(const std::string& name, const Color& color);
    Color UV(double u, double v) const override;
};

class Gradient : public Texture {
public:
    Gradient(const std::string& name, const Color& color);
    Color UV(double u, double v) const override;
};

class ImageWrapper : public Texture {
public:
    ImageWrapper(const std::string& name, const Color& color, std::string filename);
    Color UV(double u, double v) const override;

    unsigned width, height;
    std::vector<int> image;
};