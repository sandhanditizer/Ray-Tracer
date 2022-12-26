#include "pixels.h"
#include "_lodepng.h"
#include <fstream>
#include <cmath>


Pixels::Pixels(const int columns, const int rows)
    :columns{columns}, rows{rows}, values(rows*columns) {
        if (columns <= 0 || rows <= 0) {
            throw std::invalid_argument("Pixel arguments for `columns` or `rows` is less than or equal to 0.\n");
        }
    }

const Color& Pixels::operator()(const int col, const int row) const {
    if (col >= columns || row >= rows) {
        throw std::invalid_argument("Out of bounds: Pixel(column, row) DNE.\n");
    }
    return values[columns * row + col];
}

Color& Pixels::operator()(const int col, const int row) {
    if (col >= columns || row >= rows) {
        throw std::invalid_argument("Out of bounds: Pixel(column, row) DNE.\n");
    }
    return values[columns * row + col];
}

void Pixels::save_ppm(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open ppm file: " + filename);
    }
    else {
        file << "P3\n" << columns << " " << rows << "\n" << 255 << "\n";

        for (int i = 0; i < static_cast<int>(values.size()); ++i) {
            file << to_color<unsigned>(values[i].x) << " ";
            file << to_color<unsigned>(values[i].y) << " ";
            file << to_color<unsigned>(values[i].z) << "\n";
        }
    }
}

void Pixels::save_png(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open png file: " + filename);
    }
    else {
        std::vector<unsigned char> Color;
        for (auto [r, g, b] : values) {
            Color.push_back(to_color<unsigned char>(r));
            Color.push_back(to_color<unsigned char>(g));
            Color.push_back(to_color<unsigned char>(b));
            Color.push_back(255);
        }

        unsigned error{lodepng::encode(filename, Color, columns, rows)};
        if (error) {
            std::cout << "Encoder Error: " << error << " - "<< lodepng_error_text(error) << "\n";
        }
    }
}

double gamma_correction(const double value) {
    const double gamma{1.0};
    return pow(value, 1 / gamma);
}