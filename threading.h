#pragma once

// MultiThread
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>

#include "vec3.h"
#include "color.h"
#include "ray.h"

using namespace std;

double hit_sphere(const point3 &center, double radius, const ray &r)
{
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
        return -1; // no intersection
    else
        return (-half_b - sqrt(discriminant)) / a;
}

color ray_color(const ray &r)
{
    auto t = hit_sphere(point3(0, 0, -1), 0.5, r);
    if (t > 0.0)
    {
        vec3 n = (r.at(t) - vec3(0, 0, -1)) / 0.5;
        return (n + 1.0) / 2.0;
    }

    vec3 unit_direction = normalize(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

// Line Counter for MultiThreading
atomic<int> line_rendered = 0;
// Funcs for supporting MultiThreading (One thread handle a line)
void threading_func(point3 camera_pos, point3 pixel00_pos, vec3 delta_u, vec3 delta_v, int index_row, int length_row, color **buffer)
{
    for (int i = 0; i < length_row; ++i)
    {
        auto pixel_center = pixel00_pos + i * delta_u + index_row * delta_v;
        auto ray_direction = pixel_center - camera_pos;

        ray primary_ray(camera_pos, ray_direction);

        color pixel_color = ray_color(primary_ray);

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
    timer = timer + chrono::microseconds(500);
    this_thread::sleep_until(timer);
    threading_indicator_func(total_lines);
}