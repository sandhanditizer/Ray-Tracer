#pragma once

#include "camera.h"
#include "pixels.h"
#include "world.h"
#include "material.h"
#include "texture.h"
#include <fstream>
#include <iostream>
#include <map>


using Materials = std::map<std::string, Material*>;
using Textures = std::map<std::string, Texture*>;

class Parser {
public:
    Parser(const std::string& filename);
    ~Parser();
    Camera get_camera();
    World get_world();
    Pixels get_pixels();
    std::string get_output_filename();

    int depth, samples, threads;
private:
    std::string input_filename, output_filename;
    bool found_camera, found_pixels, found_output, found_rays;
    Point3D camera_position, camera_target;
    Vector3D camera_up;
    double camera_fov, aspect;
    int columns, rows;
    World world;
    Materials materials;
    Textures texture;

    void parse(std::ifstream& input);
    void verify();
    void parse_camera(std::stringstream& ss);
    void parse_pixels(std::stringstream& ss);
    void parse_sphere(std::stringstream& ss);
    void parse_triangle(std::stringstream& ss);
    void parse_output(std::stringstream& ss);
    void parse_rays(std::stringstream& ss);
    void parse_threads(std::stringstream& ss);
    void parse_material(std::stringstream& ss);
    void parse_mesh(std::stringstream& ss);
};

void remove_comment(std::string& line);
bool is_whitespace(const std::string& line);

Point3D mesh_average(std::vector<Point3D> vertices);
void Z_rotate(Point3D& vertex, const Point3D& mean, double theta);