#include <iostream>
#include <fstream>

#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "thraeding.h"

using namespace std;

int main(int argc, char const *argv[])
{

    // Image
    ofstream o("./image.ppm", ios::out);
    auto aspect_ratio = 16.0 / 9.0;
    int img_width = 1600;

    int img_height = static_cast<int>(img_width / aspect_ratio);
    img_height = (img_height < 1) ? 1 : img_height; // img_height must > 1

    // Camera
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * static_cast<double>(img_width / img_height);
    auto camera_center = point3(0, 0, 0);

    // Vectors of viewport delta_pixel offsets
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);
    auto pixel_delta_u = viewport_u / img_width;
    auto pixel_delta_v = viewport_v / img_height;

    // Pixel position of upper left pixel (index 00)
    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    auto pixel00_position = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Try MultiThreading
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
    if (o.is_open())
    {
        o << "P3\n"
          // P3 means color in ASCII
          << img_width << ' ' << img_height << "\n255" << endl;

        for (int j = 0; j < img_height; ++j)
        {
            // clog << "\rRendered Lines: " << j << " / " << img_height << flush;
            // for (int i = 0; i < img_width; ++i)
            // {
            //     auto pixel_center = pixel00_position + i * pixel_delta_u + j * pixel_delta_v;
            //     auto ray_direction = pixel_center - camera_center;

            //     ray primary_ray(camera_center, normalize(ray_direction));

            //     color pixel_color = ray_color(primary_ray);
            //     write_color(o, pixel_color);
            // }

            // Multi Threading
            threads.emplace_back(threading_func, camera_center, pixel00_position, pixel_delta_u, pixel_delta_v, j, img_width, buffer);
        }

        for (int j = 0; j < img_height; ++j)
            threads[j].join();

        thread thread_indicator(threading_indicator_func, img_height);
        thread_indicator.join();

        clog << "Calculation Done. Now Transferring Data to target.ppm" << endl;

        // Transfer color from buffer to img
        for (int j = 0; j < img_height; ++j)
        {
            for (int i = 0; i < img_width; ++i)
            {
                write_color(o, buffer[j][i]);
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

    for (int i = 0; i < img_height; ++i)
    {
        delete[] buffer[i];
    }
    delete[] buffer;
    threads.clear();

    o.close();
    std::clog << "\nDone. Rendering Time: " << rendering_time.count() << 's' << endl;
}
