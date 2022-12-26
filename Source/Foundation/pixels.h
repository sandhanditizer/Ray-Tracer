#pragma once

#include "color.h"
#include <algorithm>
#include <vector>


class Pixels {
public:
    Pixels(const int columns, const int rows);

    const Color& operator()(const int row, const int col) const;
    Color& operator()(const int row, const int col);
  
    void save_ppm(const std::string& filename); // Save as ppm image.
    void save_png(const std::string& filename); // Save as png image.
  
    const int columns, rows;
    std::vector<Color> values;
};

double gamma_correction(const double value);

template <class T>
T to_color(const double value) {
    double clamped{std::clamp(value, 0.0, 0.999)}; // Clamp value in range [0, 1).
    clamped = gamma_correction(clamped);
    return static_cast<T>(256 * clamped); // Scale to 8-bit color value.
}