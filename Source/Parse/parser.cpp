#include "parser.h"
#include "texture.h"
#include "constants.h"
#include <sstream>
#include <memory>
#include <cmath>
#include <optional>


Parser::Parser(const std::string& filename)
    :input_filename{filename}, found_camera{false}, found_pixels{false}, found_output{false}, found_rays{false} {
        std::ifstream input{filename};
        if (!input) {
            throw std::runtime_error("Cannot open file " + filename + " for parsing.\n");
        }
        parse(input);
        verify();
    }

Parser::~Parser() {
    // For textures
    for (auto iter = texture.begin(); iter != texture.end(); ++iter) {
        delete iter->second;
    }
    // For materials
    for (auto iter = materials.begin(); iter != materials.end(); ++iter) {
        delete iter->second;
    }
}

Camera Parser::get_camera() {
    return Camera{camera_position, camera_target, camera_up, camera_fov, aspect};
}

World Parser::get_world() {
    return world;
}

Pixels Parser::get_pixels() {
    return Pixels{columns, rows};
}

std::string Parser::get_output_filename() { 
    return output_filename;
}

void Parser::parse(std::ifstream& input) {
    std::string line;
    while (std::getline(input, line)) {
        remove_comment(line);
        if (is_whitespace(line)) {
            continue;
        }

        std::stringstream ss{line};
        std::string type;
        ss >> type;

        if (type == "camera") {
            parse_camera(ss);
        }
        else if (type == "pixels") {
            parse_pixels(ss);
        }
        else if (type == "sphere") {
            parse_sphere(ss);
        }
        else if (type == "triangle") {
            parse_triangle(ss);
        }
        else if (type == "output") {
            parse_output(ss);
        }
        else if (type == "rays") {
            parse_rays(ss);
        }
        else if (type == "threads") {
            parse_threads(ss);
        }
        else if (type == "material") {
            parse_material(ss);
        }
        else if (type == "mesh") {
            parse_mesh(ss);
        }
        else {
            throw std::invalid_argument("Unrecognized type found in the first argument of line: " + line + ".\n");
        }
    }
}

void Parser::verify() {
    if (!found_camera) {
        throw std::runtime_error(input_filename + " is missing camera.\n");
    }
    if (!found_pixels) {
        throw std::runtime_error(input_filename + " is missing pixels.\n");
    }
    if (output_filename.empty()) {
        throw std::runtime_error(input_filename + " is missing output.\n");
    }
    if (world.objects.empty()) {
        throw std::runtime_error(input_filename + " is missing shapes.\n");
    }
    if (!found_rays) {
        throw std::runtime_error(input_filename + " is missing rays.\n");
    }
    if (materials.empty()) {
        throw std::runtime_error(input_filename + " is missing materials.\n");
    }
} 

void Parser::parse_camera(std::stringstream& ss) {
    if (ss >> camera_position >> camera_target >> camera_up >> camera_fov) {
        found_camera = true;
    }
    else {
        throw std::invalid_argument("Camera was malformed in " + input_filename + ".\n");
    }
}

void Parser::parse_pixels(std::stringstream& ss) {
    if (ss >> columns >> rows) {
        found_pixels = true;
        aspect = static_cast<double>(columns) / rows;
    }
    else {
        throw std::invalid_argument("Pixels was malformed in " + input_filename + ".\n");
    }
}

void Parser::parse_sphere(std::stringstream& ss) {
    Vector3D center;
    double radius;
    std::string material_name;

    if (ss >> center >> radius >> material_name) {
        auto m{materials.find(material_name)};
        if (m != materials.end()) {
            world.add(Sphere{center, radius, m->second});
        }
        else {
            throw std::invalid_argument("Unknown sphere material name: " + material_name + ".\n");
        }
    }
    else {
        throw std::invalid_argument("Sphere was malformed in " + input_filename + ".\n");
    }
}

void Parser::parse_triangle(std::stringstream& ss) {
    Vector3D v0, v1, v2;
    double theta;
    std::string material_name;
    if (ss >> v0 >> v1 >> v2 >> material_name >> theta) {
        Point3D avg = mesh_average(std::vector<Point3D>{v0, v1, v2});
        Z_rotate(v0, avg, theta);
        Z_rotate(v1, avg, theta);
        Z_rotate(v2, avg, theta);

        auto m{materials.find(material_name)};
        if (m != materials.end()) {
            world.add(Triangle{v0, v1, v2, m->second});
        }
        else {
            throw std::invalid_argument("Unknown triangle material name: " + material_name + ".\n");
        }
    }
    else {
        throw std::invalid_argument("Triangle was malformed in " + input_filename + ".\n");
    }
}

void Parser::parse_output(std::stringstream& ss) {
    if ((ss >> output_filename)) {
        found_output = true;
    }
    else {
        throw std::runtime_error(input_filename + " has malformed output filename.\n");
    }
}

void Parser::parse_rays(std::stringstream& ss) {
    if (ss >> samples >> depth) {
        found_rays = true;
    }
    else {
        throw std::runtime_error(input_filename + " has malformed rays.\n");
    }
}

void Parser::parse_threads(std::stringstream& ss) {
    if (ss >> threads) {
        if (threads < 1) {
            throw std::invalid_argument("Thread argument is less than 1.\n");
        }
    }
    else {
        throw std::runtime_error(input_filename + " has malformed threads.\n");
    }
}

void Parser::parse_material(std::stringstream& ss) {
    std::string name, kind, texture_name;
    Color color;
    bool emitting;

    if (ss >> name >> kind >> texture_name >> color >> std::boolalpha >> emitting) {
        if (texture_name == "solid") {
            texture[texture_name] = new Solid(texture_name, color);
        }
        else if (texture_name == "checkerboard") {
            texture[texture_name] = new Checkerboard(texture_name, color);
        }
        else if (texture_name == "gradient") {
            texture[texture_name] = new Gradient(texture_name, color);
        }
        else if (texture_name == "imagewrap") {
            std::string image_file;
            if (ss >> image_file) {
                std::ifstream input("../../Test/Input_Files/" + image_file);
                if(!input) {
                    throw std::runtime_error("Cannot open imagewrapper file: " + image_file + ".\n");
                }
                texture[texture_name] = new ImageWrapper(texture_name, color, "../../Test/Input_Files/" + image_file);
            }
            else {
                throw std::invalid_argument("Malformed image texture.\n");
            }
        }
        else {
            throw std::invalid_argument("Unknown texture name: " + texture_name + ".\n");
        }


        if (kind == "diffuse") {
            materials[name] = new Diffuse(emitting, texture[texture_name]);
        }
        else if (kind == "specular") {
            materials[name] = new Specular(emitting, texture[texture_name]);
        }
        else if (kind == "metal") {
            double fuzz;
            if (ss >> fuzz) {
                materials[name] = new Metal(emitting, fuzz, texture[texture_name]);
            }
            else {
                throw std::runtime_error("Missing fuzz parameter for metal.\n");
            }
        }
        else if (kind == "glass") {
            double index;
            if (ss >> index) {
                materials[name] = new Glass(emitting, index, texture[texture_name]);
            }
            else {
                throw std::runtime_error("Missing index of refraction parameter for glass.\n");
            }
        }
        else if (kind == "birefringence") {
            double index1, index2;
            if (ss >> index1 >> index2) {
                materials[name] = new Birefringence(emitting, index1, index2, texture[texture_name]);
            }
            else {
                throw std::runtime_error("Missing index of refraction parameter for birefringence.\n");
            }
        }
        else {
            throw std::invalid_argument("Unknown material name: " + kind + ".\n");
        }
    }
    else {
        throw std::runtime_error(input_filename + " has a malformed material named " + name + ".\n");
    }
}

void Parser::parse_mesh(std::stringstream& ss) {
    Vector3D position;
    std::string filename, material_name;
    double theta, scale;
    if (!(ss >> position >> filename >> material_name >> theta >> scale)) {
        throw std::runtime_error("Malformed mesh line.\n");
    }

    std::ifstream input{"../../Test/Meshes/" + filename};
    if (!input) {
        throw std::runtime_error("Cannot open mesh file: " + filename + ".\n");
    }


    std::string line;
    input >> line;
    if (line != "vertices") {
        throw std::runtime_error("Mesh file must start with string 'vertices'.\n");
    }


    std::vector<Vector3D> vertices;
    for (Vector3D vertex; input >> vertex;) {
        vertices.push_back(vertex + position);
    }
    if (vertices.size() < 3) {
        throw std::runtime_error("Mesh file must contain at least 3 vertices.\n");
    }

    Point3D avg = mesh_average(vertices);
    for (auto& v : vertices) {
        Z_rotate(v, avg, theta);
        v *= scale;
    }

    input.clear(); // Clear error bit because "triangles" was read
    input >> line; // Read "triangles"

    Material* material = materials[material_name];

    // Read each triangle line
    for (int a, b, c; input >> a >> b >> c;) { 
        if (a < static_cast<int>(vertices.size()) && b < static_cast<int>(vertices.size()) && c < static_cast<int>(vertices.size())) {
            world.add(Triangle{vertices.at(a), vertices.at(b), vertices.at(c), material});
        }
        else {
            throw std::invalid_argument("Incorrect meshing of triangles in " + filename + ".\n");
        }
    }
}

void remove_comment(std::string& line) {
    auto i{line.find('#')};
    if (i != std::string::npos) {
        line = line.substr(0, i);
    }
}

bool is_whitespace(const std::string& line) {
    return std::all_of(std::begin(line), std::end(line), ::isspace);
}

Point3D mesh_average(std::vector<Point3D> vertices) {
    Point3D temp{0, 0, 0};
    for (auto v : vertices) {
        temp.x += v.x;
        temp.y += v.y;
        temp.z += v.z;
    }
    return (temp / vertices.size());
}

void Z_rotate(Point3D& vertex, const Point3D& mean, double theta) {
    // Rotates a given triangle about its centered z-axis.
    theta = (theta * Constants::Pi) / 180;
    Point3D temp = vertex - mean;
    vertex.x = (temp.x * cos(theta)) + (temp.y * -sin(theta)) + mean.x;
    vertex.y = (temp.x * sin(theta)) + (temp.y * cos(theta)) + mean.y;
}