#pragma once

// MultiThread
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>

#include "rtweekend.h"
#include "hittable.h"
#include "sphere.h"
#include "hittable_list.h"
#include "color.h"

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

color ray_color(const ray &r, const hittable &world)
{
    hit_info info;

    if (world.hit(r, interval(0, infinity), info))
    {
        vec3 reflex_dir = random_on_hemisphere(info.normal);
        return 0.5 * ray_color(ray(info.hit_point, reflex_dir), world);
    }

    vec3 unit_direction = normalize(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

// Line Counter for MultiThreading
atomic<int> line_rendered(0);
// Funcs for supporting MultiThreading (One thread handle a line)
void threading_func(const hittable &world, point3 camera_pos, point3 pixel00_pos, vec3 pixel_delta_u, vec3 pixel_delta_v, int index_row, int length_row, int samplers_per_pixel, color **buffer)
{
    for (int i = 0; i < length_row; ++i)
    {
        color pixel_color(0, 0, 0);
        for (int sample = 0; sample < samplers_per_pixel; ++sample)
        {
            ray r = get_ray(camera_pos, pixel00_pos, pixel_delta_u, pixel_delta_v, index_row, i);
            pixel_color += ray_color(r, world);
        }
        
        // Write all color into buffer
        buffer[index_row][i] = pixel_color;
    }

    ++line_rendered;
}

void threading_indicator_func(int total_lines)
{
    if (line_rendered >= total_lines)
    {
        return;
    }

    clog << "\rRendered Lines: " << line_rendered << " / " << total_lines << flush;

    auto timer = chrono::steady_clock::now();
    timer = timer + chrono::microseconds(20);
    this_thread::sleep_until(timer);
    threading_indicator_func(total_lines);
}