#pragma once

#include "rtweekend.h"
#include "thread"

#include "hittable_list.h"
#include "material.h"
#include "threading.h"

class camera
{
public:
    double aspect_ratio = 1.0;          // Ratio of image width over height
    int image_width = 1;                // Rendered image width in pixel count
    int samplers_per_pixel = 10;        // Amount of samplers for each pixel
    double ray_gen_probability = 0.6;   // Probability of ray generation. (Instead of using max depth, let's try Russian Roulette!)

    double vfov = 90;                   // Vertical field of view
    point3 lookfrom = point3(0, 0, -1); // Point where camera is looking from
    point3 lookat = point3(0, 0, 0);    // Point where camera is looking at
    vec3 vup = vec3(0, 1, 0);           // Absolute up direction (world space)

    double defocus_angle = 0;           // Variation angle of rays through each pixel
    double focus_dist = 10;             // Distance form camera lookfrom point to perfect focus plane
    double frame_duration = 1.0;        // Shutter opening time

    color background = color(0, 0, 0);  // Scene background color (more like env light actually, could add HDRI or cube_map support someday)

    // Rendering
    void render(const hittable_list /* don't get it, but it works, and it won't work with 'const hittable' here*/ &world)
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
            threads.emplace_back(&camera::render_line, this, world, j, buffer);
        }

        thread thread_indicator(threading::threading_indicator, image_height);
        thread_indicator.detach();

        for (int j = 0; j < image_height; ++j)
            if (threads[j].joinable())
                threads[j].join();

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
    int image_height;    // Rendered image height
    point3 center;       // Camera center
    point3 pixel00_pos;  // Position of pixel (0, 0)
    vec3 pixel_delta_u;  // Offset to pixel to the right
    vec3 pixel_delta_v;  // Offset to pixel below
    vec3 u, v, w;        // Camera frame basis vectors
    vec3 defocus_disk_u; // Defocus disk horizontal radius
    vec3 defocus_disk_v; // Defocus disk vertical radius

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

    // Return a random offset in the square around pixel
    vec3 pixel_sample_square()
    {
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();

        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    // Return a random point in the camera defocus disk
    point3 defocus_disk_sample()
    {
        auto p = random_in_unit_disk();
        return center + p[0] * defocus_disk_u + p[1] * defocus_disk_v;
    }

    // Return a randomly sampled ray for pixel i, j, originating from the camera defocus disk
    ray get_primary_ray(int i, int j)
    {
        auto pixel_center = pixel00_pos + (j * pixel_delta_u) + (i * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin = defocus_angle <= 0 ? center : defocus_disk_sample();

        auto ray_direction = pixel_sample - ray_origin;

        // Launch rays in a shutter opening duration
        double ray_time = random_double(0, frame_duration);
        return ray(ray_origin, ray_direction, ray_time);
    }

    color ray_color(const ray &r, const hittable &world, double ray_gen_probability)
    {
        hit_info hit;

        // Simply address the floating point error on intersection by ignoring intersecting point which is close enough to surfaces
        if (world.hit(r, interval(0.001, infinity), hit))
        {
            double p = random_double();
            if (p < ray_gen_probability)
            {
                ray scattered;
                color attenuation;
                color emission_color = hit.mat->emitter(hit.u, hit.v, hit.hit_point);

                if (!hit.mat->scatter(r,hit,attenuation,scattered))
                    return emission_color;

                color scatter_color = attenuation * ray_color(scattered, world, ray_gen_probability);
                return emission_color + scatter_color;
            }
            return color();
        }
        // If ray hits nothing, simply return background color
        else
        {
            return background;
        }
    }

    // Funcs for MultiThreading (each thread handle a line)
    void render_line(const hittable &world, int index_row, color **const buffer)
    {
        for (int i = 0; i < image_width; ++i)
        {
            color pixel_color(0, 0, 0);
            for (int sample = 0; sample < samplers_per_pixel; ++sample)
            {
                ray r = get_primary_ray(index_row, i);
                pixel_color += ray_color(r, world, ray_gen_probability);
            }

            // Write all color into buffer
            buffer[index_row][i] = pixel_color;
        }

        ++threading::thread_finished;
    }
};