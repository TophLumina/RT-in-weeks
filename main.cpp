#include <iostream>
#include <fstream>

#include "rtweekend.h"

#include "color.h"
#include "threading.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

using namespace std;

int main(int argc, char const *argv[])
{

    // Image
    ofstream out("./image.ppm", ios::out);
    auto aspect_ratio = 16.0 / 9.0;
    int img_width = 800;

    int img_height = static_cast<int>(img_width / aspect_ratio);
    img_height = (img_height < 1) ? 1 : img_height; // img_height must > 1

    // World

    hittable_list world;

    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * static_cast<double>(img_width) / img_height;
    auto camera_center = point3(0, 0, 0);

    // Vectors of viewport delta_pixel offsets
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);
    auto pixel_delta_u = viewport_u / img_width;
    auto pixel_delta_v = viewport_v / img_height;

    // Pixel position of upper left pixel (index 00)
    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    auto pixel00_position = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Buffer for MultiThreading
    color **buffer = new color *[img_height];
    for (int i = 0; i < img_height; ++i)
    {
        buffer[i] = new color[img_width];
    }

    // Threads
    vector<thread> threads;

    // Timer
    auto start = chrono::steady_clock::now();

    // Render
    if (out.is_open())
    {
        out << "P3\n"
            // P3 means color in ASCII
            << img_width << ' ' << img_height << "\n255" << endl;

        for (int j = 0; j < img_height; ++j)
        {
            // Load Threads
            threads.emplace_back(threading_func, world, camera_center, pixel00_position, pixel_delta_u, pixel_delta_v, j, img_width, buffer);
        }

        for (int j = 0; j < img_height; ++j)
            threads[j].join();

        thread thread_indicator(threading_indicator_func, img_height);
        thread_indicator.detach();

        clog << "Calculation Done. Now Transferring Data to target.ppm" << endl;

        // Transfer color from buffer to img
        for (int j = 0; j < img_height; ++j)
        {
            for (int i = 0; i < img_width; ++i)
            {
                write_color(out, buffer[j][i]);
            }
        }
    }
    else
    {
        std::clog << "File dead." << endl;
        return 1;
    }

    auto transfer_end = chrono::steady_clock::now();
    auto rendering_time = chrono::duration_cast<chrono::seconds>(transfer_end - start);

    world.clear();
    for (int i = 0; i < img_height; ++i)
    {
        delete[] buffer[i];
    }
    delete[] buffer;
    threads.clear();

    out.close();
    std::clog << "\nTransfer Done. Total Rendering Time: " << rendering_time.count() << 's' << endl;
}
