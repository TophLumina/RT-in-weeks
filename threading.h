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

// Return a random offset in the square around pixel
vec3 pixel_sample_square(vec3 pixel_delta_u, vec3 pixel_delta_v)
{
    auto px = -0.5 + random_double();
    auto py = -0.5 + random_double();

    return (px * pixel_delta_u) + (py * pixel_delta_v);
}

// Return a randomly sampled ray for pixel i, j
ray get_ray(point3 origin, point3 pixel00_pos, vec3 pixel_delta_u, vec3 pixel_delta_v, int i, int j)
{
    auto pixel_center = pixel00_pos + (j * pixel_delta_u) + (i * pixel_delta_v);
    auto pixel_sample = pixel_center + pixel_sample_square(pixel_delta_u, pixel_delta_v);

    auto ray_direction = pixel_sample - origin;

    return ray(origin, ray_direction);
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

            if (hit.mat -> scatter(r, hit, attenuation, scattered))
                return attenuation * ray_color(scattered, world, ray_gen_probability);
        }
        return color();
    }

    vec3 unit_direction = normalize(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0); // Serve as Sky Color ()
}

// Line Counter for MultiThreading
static atomic<int> line_rendered(0);
// Funcs for supporting MultiThreading (One thread handle a line)
void threading_func(const hittable &world, point3 camera_pos, point3 pixel00_pos, vec3 pixel_delta_u, vec3 pixel_delta_v, int index_row, int length_row, int samplers_per_pixel, double ray_gen_probability, color **buffer)
{
    for (int i = 0; i < length_row; ++i)
    {
        color pixel_color(0, 0, 0);
        for (int sample = 0; sample < samplers_per_pixel; ++sample)
        {
            ray r = get_ray(camera_pos, pixel00_pos, pixel_delta_u, pixel_delta_v, index_row, i);
            pixel_color += ray_color(r, world, ray_gen_probability);
        }

        // Write all color into buffer
        buffer[index_row][i] = pixel_color;
    }

    ++line_rendered;
}

void threading_indicator_func(int total_lines)
{
    while (line_rendered < total_lines)
    {
        clog << "\rLines Rendered: " << line_rendered << " / " << total_lines << flush;
        this_thread::sleep_for(chrono::microseconds(250));
    }
}