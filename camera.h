#pragma once

#include <thread>
#include "rtweekend.h"

#include "hittable_list.h"
#include "material.h"
#include "threading.h"
#include "PDF.h"
#include "FrameBuffer.h"

class camera
{
public:
    double aspect_ratio = 1.0;   // Ratio of image width over height
    int image_width = 1;         // Rendered image width in pixel count
    int samplers_per_pixel = 16; // Amount of samplers for each pixel
    int max_depth = 20;          // Ray bounce limit

    double vfov = 90;                   // Vertical field of view
    point3 lookfrom = point3(0, 0, -1); // Point where camera is looking from
    point3 lookat = point3(0, 0, 0);    // Point where camera is looking at
    vec3 vup = vec3(0, 1, 0);           // Absolute up direction (world space)

    double defocus_angle = 0;    // Variation angle of rays through each pixel
    double focus_dist = 10;      // Distance form camera look_from point to perfect focus plane
    double frame_duration = 1.0; // Shutter opening time

    color background = color(0, 0, 0); // Scene background color (more like env light actually, could add HDRI or cube_map support someday)

    void render(const hittable &world, const hittable &lights)
    {
        initialize();

        // Buffer
        FrameBuffer<color> color_buffer(image_width, image_height);

        // Threads
        vector<thread> threads;

        // Timer
        auto start = chrono::steady_clock::now();

        for (int j = 0; j < image_height; ++j)
        {
            // Load Threads
            threads.emplace_back(&camera::render_line, this, cref(world), j, color_buffer.data, cref(lights));
        }

        thread thread_indicator(threading::threading_indicator, image_height);
        thread_indicator.detach();

        for (int j = 0; j < image_height; ++j)
            if (threads[j].joinable())
                threads[j].join();

        auto trace_end = chrono::steady_clock::now();
        auto tracing_time = chrono::duration_cast<chrono::seconds>(trace_end - start);
        clog << "\rTracing Completed. Ray Tracing Time: " << tracing_time.count() << "s" << endl;

        // PPM output
        cout
            << "P3\n"
            << image_width << ' ' << image_height << "\n255\n";
        for (int i = 0; i < image_height; ++i)
        {
            for (int j = 0; j < image_width; ++j)
            {
                write_color(cout, color_buffer.data[i][j], samplers_per_pixel);
            }
        }

        // PNG output
        int comp = 3;
        function<void(color, unsigned char *)> trans = [&](color c, unsigned char *p) -> void
        {
            for (int i = 0; i < comp; ++i)
            {
                int val = static_cast<int>(255.99 *const interval(0.000, 0.999).clamp(linear2gamma(c.e[i] / samplers_per_pixel)));
                p[i] = static_cast<unsigned char>(val);
            }
            return;
        };
        rtw_image image(color_buffer, comp, trans);
        image.saveasPNG("./image.png");

        auto transfer_end = chrono::steady_clock::now();
        auto rendering_time = chrono::duration_cast<chrono::seconds>(transfer_end - start);

        std::clog << "Data Transfer Completed. Total Rendering Time: " << rendering_time.count() << 's' << endl;

        // Releasing resource
        threads.clear();
        // delete[] image_data;
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
    int sqrt_spp;        // Subpixel var for pixel sample stratifying
    double stride_spp;   // Stride of subpixel stratifying

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

        // Subpixel stratifying
        sqrt_spp = static_cast<int>(sqrt(samplers_per_pixel));
        stride_spp = 1.0 / sqrt_spp;
    }

    // Return a random offset in the square around pixel, given two sub-pixel indexes
    vec3 pixel_sample_square(int sub_i, int sub_j) const
    {
        auto px = -0.5 + stride_spp * (sub_i + random_double());
        auto py = -0.5 + stride_spp * (sub_j + random_double());

        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    // Return a random point in the camera defocus disk
    point3 defocus_disk_sample() const
    {
        auto p = random_in_unit_disk();
        return center + p[0] * defocus_disk_u + p[1] * defocus_disk_v;
    }

    // Return a randomly sampled ray for pixel i, j and given subpixel indexes, originating from the camera defocus disk
    ray get_primary_ray(int i, int j, int sub_i, int sub_j) const
    {
        auto pixel_center = pixel00_pos + (j * pixel_delta_u) + (i * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square(sub_i, sub_j);

        auto ray_origin = defocus_angle <= 0 ? center : defocus_disk_sample();

        auto ray_direction = pixel_sample - ray_origin;

        // Launch rays in a shutter opening duration
        double ray_time = random_double(0, frame_duration);
        return ray(ray_origin, ray_direction, ray_time);
    }

    color ray_color(const ray &r, const hittable &world, int current_depth, const hittable &lights) const
    {
        hit_info hit;

        // Simply address the floating point error on intersection by ignoring intersecting point which is close enough to surfaces
        if (world.hit(r, interval(0.001, infinity), hit))
        {
            --current_depth;
            if (current_depth > 0)
            {
                ray scattered;
                color attenuation;
                double pdf_val;
                color emission_color = hit.mat->emitter(r, hit, hit.u, hit.v, hit.hit_point);

                if (!hit.mat->scatter(r, hit, attenuation, scattered, pdf_val))
                    return emission_color;

                auto surface_pdf = make_shared<cosine_hemisphere_pdf>(hit.normal);
                auto light_pdf = make_shared<hittable_pdf>(lights, hit.hit_point);
                mixture_pdf mixed_pdf(surface_pdf, light_pdf, 0.75);

                scattered = ray(hit.hit_point, mixed_pdf.generate(), r.time());
                pdf_val = mixed_pdf.value(scattered.direction());

                double scattering_pdf = hit.mat->scattering_pdf(r, hit, scattered);

                color sample_color = ray_color(scattered, world, current_depth, lights);
                color scatter_color = (attenuation * scattering_pdf * sample_color) / pdf_val;

                return emission_color + scatter_color;
            }

            return color(0, 0, 0);
        }
        // If ray hits nothing, simply return background color
        else
        {
            return background;
        }
    }

    // Funcs for MultiThreading (each thread handle a line)
    void render_line(const hittable &world, int index_row, color **const buffer, const hittable &lights)
    {
        for (int i = 0; i < image_width; ++i)
        {
            color pixel_color(0, 0, 0);

            for (int s_i = 0; s_i < sqrt_spp; ++s_i)
            {
                for (int s_j = 0; s_j < sqrt_spp; ++s_j)
                {
                    ray r = get_primary_ray(index_row, i, s_i, s_j);
                    pixel_color += ray_color(r, world, max_depth, lights);
                }
            }

            // Write all color into buffer
            buffer[index_row][i] = pixel_color;
        }

        ++threading::thread_finished;
    }
};