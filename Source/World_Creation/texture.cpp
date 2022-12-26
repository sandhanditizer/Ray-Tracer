#include "texture.h"
#include "random.h"
#include "_lodepng.h"
#include "pixels.h"
#include <cmath>
#include <vector>


Texture::Texture(const std::string& name, const Color& color)
    :name{name}, color{color} {}

Texture::~Texture() {}

// ----------------------------------------------------------------------

Solid::Solid(const std::string& name, const Color& color)
    :Texture{name, color} {}

Color Solid::UV(double, double) const {
    return color;
}

// ----------------------------------------------------------------------

Checkerboard::Checkerboard(const std::string& name, const Color& color)
    :Texture{name, color} {}

Color Checkerboard::UV(double u, double v) const {
    double sines = sin(u * 100) * sin(v * 100);
    if (sines < 0) {
        return color;
    }
    else {
        return White - color; // Color inverse.
    }
}

// ----------------------------------------------------------------------

Gradient::Gradient(const std::string& name , const Color& color)
    :Texture{name, color} {}

Color Gradient::UV(double u, double v) const {
    return Color{color.x * u, color.y * v, color.z * (1 - u - v)};
}

// ----------------------------------------------------------------------


ImageWrapper::ImageWrapper(const std::string& name, const Color& color, std::string filename)
    :Texture{name, color} {
        std::vector<unsigned char> image_vec;
        unsigned error = lodepng::decode(image_vec, width, height, filename);
        if (error) {
            throw std::invalid_argument("Error occured while decoding png file: " 
            + static_cast<std::string>(lodepng_error_text(error)) 
            + "\n");
        }
        for (int i : image_vec) {
            image.push_back(i);
        }
    }

Color ImageWrapper::UV(double u, double v) const  {
    if (u < 0.0 || v < 0.0 || u > 1.0 || v > 1.0) {
        return Color{0, 0, 0};
    }
    int x = static_cast<int>(u * width);
    int y = static_cast<int>(v * height);
    int index = y * width + x;
    return Color{image[index * 4] * 1.0, image[index * 4 + 1] * 1.0, image[index * 4 + 2] * 1.0} / 255.0;
}