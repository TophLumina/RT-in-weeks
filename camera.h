#pragma once

#include "rtweekend.h"

#include "threading.h"

class camera
{
public:
    double aspect_ratio = 1.0;   // Ratio of image width over height
    int image_width = 1;         // Rendered image width in pixel count
    int samplers_per_pixel = 64;// Amount of samplers for each pixel

    void render(const hittable_list/* don't get it, but it works, and it won't work when get const hittable here*/ &world)
    {
        initialize();

        // Buffer
        color **buffer = new color *[image_height];
        for (int i = 0; i < image_height; ++i)
        {
            buffer[i] = new color[image_width];
        }

        // Threads
        vector<thread> threads;

        // Timer
        auto start = chrono::steady_clock::now();

        // Rendering
        cout << "P3\n"
             << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j)
        {
            // Load Threads
            threads.emplace_back(threading_func, world, center, pixel00_pos, pixel_delta_u, pixel_delta_v, j, image_width, samplers_per_pixel, buffer);
        }

        thread thread_indicator(threading_indicator_func, image_height);
        thread_indicator.detach();

        for (int j = 0; j < image_height; ++j)
            if (threads[j].joinable())
                threads[j].join();

        clog << "\rCalculation Done. Now Transferring Data to target.ppm" << endl;

        // Transfer data from buffer to img
        for (int j = 0; j < image_height; ++j)
        {
            for (int i = 0; i < image_width; ++i)
            {
                write_color(cout, buffer[j][i], samplers_per_pixel);
            }
        }

        auto transfer_end = chrono::steady_clock::now();
        auto rendering_time = chrono::duration_cast<chrono::seconds>(transfer_end - start);

        std::clog << "\nDone. Total Rendering Time: " << rendering_time.count() << 's' << endl;

        // Releasing resource
        for (int i = 0; i < image_height; ++i)
        {
            delete[] buffer[i];
        }
        delete[] buffer;
        threads.clear();
    }

private:
    int image_height;   // Rendered image height
    point3 center;      // Camera center
    point3 pixel00_pos; // Position of pixel (0, 0)
    vec3 pixel_delta_u; // Offset to pixel to the right
    vec3 pixel_delta_v; // Offset to pixel below

    void initialize()
    {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = point3(0, 0, 0);

        // Viewport
        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * static_cast<double>(image_width) / image_height;

        //  Viewport delta offsets
        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);

        // Viewport delta pixel_offsets
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Position of pixel (0, 0)
        auto viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        pixel00_pos = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }
};
