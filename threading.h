#pragma once

// MultiThread
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>

#include "rtweekend.h"
#include "sphere.h"
#include "hittable_list.h"
#include "material.h"

using namespace std;

struct line_renderer_data_block
{
    point3 camera_pos;
    point3 pixel00_pos;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    int index_row;
    int length_row;
    int samplers_per_pixel;
    double ray_gen_probability;
    double defocus_angle;
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;
};

class threading // now served as a helper function lib to achieve parallelism, might be reconstructed into thread pool someday
{
public:
    // Funcs for supporting MultiThreading (One thread handle a line)
    static void threading_line_renderer(const hittable &world, const line_renderer_data_block &data, color **const buffer)
    {
        for (int i = 0; i < data.length_row; ++i)
        {
            color pixel_color(0, 0, 0);
            for (int sample = 0; sample < data.samplers_per_pixel; ++sample)
            {
                ray r = get_primary_ray(data.camera_pos, data.pixel00_pos, data.pixel_delta_u, data.pixel_delta_v, data.defocus_angle, data.defocus_disk_u, data.defocus_disk_v, data.index_row, i);
                pixel_color += ray_color(r, world, data.ray_gen_probability);
            }

            // Write all color into buffer
            buffer[data.index_row][i] = pixel_color;
        }

        ++thread_finished;
    }

    static void threading_indicator(int total_lines)
    {
        while (thread_finished < total_lines)
        {
            clog << "\rLines Rendered: " << thread_finished << " / " << total_lines << flush;
            this_thread::sleep_for(chrono::microseconds(500));
        }
    }

    static void manual_reset() { thread_finished = 0; }

    // TODO:: Functions in the private section follow will have to be moved to separate headers if I decide to extend this class into some kind of thread pool :(
private:
    // Line Counter for MultiThreading
    static atomic<int> thread_finished;

    // Return a random offset in the square around pixel
    static vec3 pixel_sample_square(vec3 pixel_delta_u, vec3 pixel_delta_v)
    {
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();

        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    // Return a random point in the camera defocus disk
    static point3 defocus_disk_sample(point3 camera_pos, vec3 defocus_disk_u, vec3 defocus_disk_v)
    {
        auto p = random_in_unit_disk();
        return camera_pos + p[0] * defocus_disk_u + p[1] * defocus_disk_v;
    }

    // Return a randomly sampled ray for pixel i, j, originating from the camera defocus disk
    static ray get_primary_ray(point3 camera_pos, point3 pixel00_pos, vec3 pixel_delta_u, vec3 pixel_delta_v, double defocus_angle, vec3 defocus_disk_u, vec3 defocus_disk_v, int i, int j)
    {
        auto pixel_center = pixel00_pos + (j * pixel_delta_u) + (i * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square(pixel_delta_u, pixel_delta_v);

        auto ray_origin = defocus_angle <= 0 ? camera_pos : defocus_disk_sample(camera_pos, defocus_disk_u, defocus_disk_v);

        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    static color ray_color(const ray &r, const hittable &world, double ray_gen_probability)
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

                if (hit.mat->scatter(r, hit, attenuation, scattered))
                    return attenuation * ray_color(scattered, world, ray_gen_probability);
            }
            return color();
        }

        vec3 unit_direction = normalize(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0); // Serve as Sky Color ()
    }
};

atomic<int> threading::thread_finished(0);