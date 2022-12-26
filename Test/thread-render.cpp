#include "parser.h"
#include "random.h"
#include <cmath>
#include <vector>
#include <thread>
#include <chrono>


void print_progress(int total_rays);
void render(Pixels& result, const Camera& camera, const World& world, int samples, int depth, int num_threads);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: Missing input.\n";
        return 0;
    }
    try {
        Parser parser{"../../Test/Input_Files/" + static_cast<std::string>(argv[1])};
        World world = parser.get_world();
        Pixels pixels = parser.get_pixels();
        Camera camera = parser.get_camera();
        int depth = parser.depth;
        int samples = parser.samples;
        int num_threads = parser.threads;

        std::vector<std::thread> threads;
        std::vector<Pixels> results(num_threads, {pixels.columns, pixels.rows});

        for (int t = 1; t < num_threads; ++t) {
            std::thread th{render, std::ref(results.at(t)), camera, world, samples/num_threads, depth, num_threads};
            threads.push_back(std::move(th));
        }

        render(std::ref(results.at(0)), camera, world, samples/num_threads, depth, num_threads);

        for (auto& thread : threads) {
            thread.join();
        }


        for (int i = 0; i < pixels.columns; ++i) {
            for (int j = 0; j < pixels.rows; ++j) {
                Color color{0, 0, 0};
                for (auto& result : results) {
                    color += result(i, j);
                }
                pixels(i, j) = color / num_threads;
            }
        }
        
        std::string filename = parser.get_output_filename();
        pixels.save_png(filename);
    }
    catch (const std::exception& err) {
        std::cout << err.what() << '\n';
    }
}

void print_progress(int total_rays) {
    static int rays{0};
    rays += 1;
    if ((rays % (total_rays / 20) == 0)) {
        std::cout << "\rProgress: " << rays << " / " << total_rays << " rays traced.       " << std::flush;
    }
}


void render(Pixels& pixels, const Camera& camera, const World& world, int samples, int depth, int num_threads) {
    for (auto i = 0; i < pixels.columns; ++i) {
        for (auto j = 0; j < pixels.rows; ++j) {
            Color color{0, 0, 0};
            for (int s = 0; s < samples; ++s) {
                double x = (i + random_double()) / (pixels.columns - 1);
                double y = (j + random_double()) / (pixels.rows - 1);
                Ray ray = camera.compute_ray(x, y);
                color += trace_path(world, ray, depth);
                print_progress(pixels.columns * pixels.rows * samples * num_threads);
            }
            pixels(i, j) = color / samples;
        }
    }
}