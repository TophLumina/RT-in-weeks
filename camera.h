#pragma once

#include "rtweekend.h"
#include "thread"

#include "threading.h"

class camera
{
public:
    double aspect_ratio = 1.0;        // Ratio of image width over height
    int image_width = 1;              // Rendered image width in pixel count
    int samplers_per_pixel = 10;      // Amount of samplers for each pixel
    double ray_gen_probability = 0.6; // Probability of ray generation. (Instead of using max depth, let's try Russian Roulette!)

    double vfov = 90;                   // Vertical field of view
    point3 lookfrom = point3(0, 0, -1); // Point where camera is looking from
    point3 lookat = point3(0, 0, 0);    // Point where camera is looking at
    vec3 vup = vec3(0, 1, 0);           // Absolute up direction (world space)

    double defocus_angle = 0; // Variation angle of rays through each pixel
    double focus_dist = 10;   // Distance form camera lookfrom point to perfect focus plane

    // Rendering
    void render(const hittable_list /* don't get it, but it works, and it won't work when get const hittable here*/ &world)
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

        for (int j = 0; j < image_height; ++j)
        {
            // Load Threads
            threads.emplace_back(threading::threading_line_renderer, world, fill_data_block(j), buffer);
        }

        thread thread_indicator(threading::threading_indicator, image_height);
        thread_indicator.detach();

        for (int j = 0; j < image_height; ++j)
            if (threads[j].joinable())
                threads[j].join();

        threading::manual_reset();

        auto trace_end = chrono::steady_clock::now();
        auto tracing_time = chrono::duration_cast<chrono::seconds>(trace_end - start);
        clog << "\rTracing Completed. Ray Tracing Time: " << tracing_time.count() << "s" << endl;

        // Transfer data from buffer to img
        cout << "P3\n"
             << image_width << ' ' << image_height << "\n255\n";
        for (int j = 0; j < image_height; ++j)
        {
            for (int i = 0; i < image_width; ++i)
            {
                write_color(cout, buffer[j][i], samplers_per_pixel);
            }
        }

        auto transfer_end = chrono::steady_clock::now();
        auto rendering_time = chrono::duration_cast<chrono::seconds>(transfer_end - start);

        std::clog << "Data Transfer Completed. Total Rendering Time: " << rendering_time.count() << 's' << endl;

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
    vec3 u, v, w;       // Camera frame basis vectors
    vec3 defocus_disk_u;// Defocus disk horizontal radius
    vec3 defocus_disk_v;// Defocus disk vertical radius

    void initialize()
    {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = lookfrom;

        // Viewport
        auto theta = degree2radius(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2.0 * h * focus_dist;
        auto viewport_width = viewport_height * static_cast<double>(image_width) / image_height;

        // u, v, w Unit basis vectors for camera coordinate frame
        w = normalize(lookfrom - lookat);
        u = normalize(cross(vup, w));
        v = cross(w, u);

        //  Viewport delta offsets
        auto viewport_u = viewport_width * u;   // Vector across viewport horizontal edge
        auto viewport_v = viewport_height * -v; // Vector down viewport vertical edge

        // Viewport delta pixel_offsets
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Position of pixel (0, 0)
        auto viewport_upper_left = center - w * focus_dist - viewport_u / 2 - viewport_v / 2;
        pixel00_pos = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Defocus disk basis vectors of camera
        auto defocus_radius = focus_dist * (tan(degree2radius(defocus_angle / 2)));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    // Utility Function for MultiThreading
    line_renderer_data_block fill_data_block(int j) const
    {
        line_renderer_data_block data_block;
        data_block.index_row = j;
        data_block.length_row = image_width;
        data_block.camera_pos = lookfrom;
        data_block.pixel00_pos = pixel00_pos;
        data_block.pixel_delta_u = pixel_delta_u;
        data_block.pixel_delta_v = pixel_delta_v;
        data_block.ray_gen_probability = ray_gen_probability;
        data_block.samplers_per_pixel = samplers_per_pixel;
        data_block.defocus_angle = defocus_angle;
        data_block.defocus_disk_u = defocus_disk_u;
        data_block.defocus_disk_v = defocus_disk_v;

        return data_block;
    }
};