#pragma once

#include <atomic>
#include <chrono>
#include <cmath>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>
#include <vector>

#include "FrameBuffer.h"
#include "Math/utility/VectorOperations.hpp"
#include "PDF.h"
#include "Reservoir.h"
#include "ThreadPool.hpp"
#include "color.h"
#include "denoiser.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "rtweekend.h"
#include "vec.h"

using namespace std;
class camera
{
public:
    double aspect_ratio = 1.0;           // Ratio of image width over height
    unsigned int image_width = 1;        // Rendered image width in pixel count
    unsigned int samplers_per_pixel = 4; // Amount of samplers for each pixel
    unsigned int RIS_size = 16;          // Amount of samplers for each pixel
    unsigned int shadow_samples = 4;     // Amount of samplers for each shadow ray
    unsigned int max_depth = 4;          // Ray bounce limit

    double vfov = 90;                   // Vertical field of view
    point3 lookfrom = point3(0, 0, -1); // Point where camera is looking from
    point3 lookat = point3(0, 0, 0);    // Point where camera is looking at
    vec3 vup = vec3(0, 1, 0);           // Absolute up direction (world space)

    double defocus_angle = 0;    // Variation angle of rays through each pixel
    double focus_dist = 10;      // Distance form camera look_from point to perfect focus plane
    double frame_duration = 1.0; // Shutter opening time

    unsigned int reservoir_size = 8; // Reservoir size for each pixel

    color background = color(0, 0, 0); // Scene background color (more like env light actually, could add HDRI or cube_map support someday)

    // Buffers
    FrameBuffer<color> color_buffer;
    FrameBuffer<point3> position_buffer;
    FrameBuffer<vec3> normal_buffer;
    FrameBuffer<vec3> index_buffer;
    FrameBuffer<Reservoir<shared_ptr<hittable>>> reservoir_buffer;

    // Denoiser
    Denoiser denoiser = Denoiser(1.2, 8, pool);

    void render(const hittable &world, const hittable_list &lights)
    {
        initialize();

        // Timer
        auto start = chrono::steady_clock::now();

        for (int j = 0; j < image_height; ++j)
        {
            for (int i = 0; i < image_width; ++i)
            {
                futures.push(pool.Submit([this, i, j, &world, &lights]
                                         { render_pixel(i, j, world, lights, color_buffer.data); }));
            }
        }

        thread thread_indicator(&camera::pixel_indicator, this, image_height * image_width);
        thread_indicator.detach();

        while (!futures.empty())
        {
            futures.front().get();
            futures.pop();
        }

        auto trace_end = chrono::steady_clock::now();
        auto tracing_time = chrono::duration_cast<chrono::seconds>(trace_end - start);
        clog << "\rTracing Completed. Tracing Time: " << tracing_time.count() << "s" << endl;

        // PNG output
        int comp = 3;
        function<void(color, unsigned char *)> trans = [&](color c, unsigned char *p) -> void
        {
            for (int i = 0; i < comp; ++i)
            {
                int val = static_cast<int>(255.99 * (interval(0.000, 0.999)).clamp(Math::linear2gamma(c[i] / samplers_per_pixel)));
                p[i] = static_cast<unsigned char>(val);
            }
            return;
        };

        // Denoise
        std::clog << "Generating G-buffers..." << endl;
        generate_Gbuffers(world);
        std::clog << "G-buffers Generated." << endl;

        // G-buffers output
        rtw_image raw_image(color_buffer, comp, trans);
        raw_image.saveasPPM("./raw.ppm");

        rtw_image gbuffer_position(position_buffer, comp, trans);
        gbuffer_position.saveasPPM("./position.ppm");

        rtw_image gbuffer_normal(normal_buffer, comp, trans);
        gbuffer_normal.saveasPPM("./normal.ppm");

        std::clog << "Denoising..." << endl;
        denoiser.denoise(color_buffer, position_buffer, normal_buffer, index_buffer);
        std::clog << "Denoising Completed." << endl;

        rtw_image image(color_buffer, comp, trans);
        image.saveasPPM("./result.ppm");

        auto transfer_end = chrono::steady_clock::now();
        auto rendering_time = chrono::duration_cast<chrono::seconds>(transfer_end - start);

        std::clog << "Data Transfer Completed. Total Rendering Time: " << rendering_time.count() << 's' << endl;
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

    // Thread Pool
    ThreadPool pool;
    queue<future<void>> futures;
    atomic<int> pixel_finished = 0;

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

        // Buffers
        color_buffer = FrameBuffer<color>(image_width, image_height, color(0, 0, 0));
        position_buffer = FrameBuffer<point3>(image_width, image_height, point3(0, 0, 0), [](const point3 &a, const point3 &b) -> double
                                              { return Math::Vector::distance(a, b) / 100; });
        normal_buffer = FrameBuffer<vec3>(image_width, image_height, vec3(0, 0, 0), [](const vec3 &a, const vec3 &b) -> double
                                          { return (1.0 - Math::Vector::dot(a, b)); });
        index_buffer = FrameBuffer<vec3>(image_width, image_height, vec3(0, 0, 0), [](const vec3 &a, const vec3 &b) -> double
                                         { return (a.x == b.x && a.y == b.y) ? 0 : 1; });
        reservoir_buffer = FrameBuffer<Reservoir<shared_ptr<hittable>>>(image_width, image_height, Reservoir<shared_ptr<hittable>>(reservoir_size));
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
        auto p = random_disk();
        return center + p[0] * defocus_disk_u + p[1] * defocus_disk_v;
    }

    // Return a randomly sampled ray for pixel i, j and given subpixel indexes, originating from the camera defocus disk
    ray get_primary_ray(int i, int j, int sub_i = 0, int sub_j = 0) const
    {
        auto pixel_center = pixel00_pos + (j * pixel_delta_u) + (i * pixel_delta_v);
        auto pixel_sample = pixel_center + ((sub_i == 0 && sub_j == 0) ? vec3(0, 0, 0) : pixel_sample_square(sub_i, sub_j));

        auto ray_origin = defocus_angle <= 0 ? center : defocus_disk_sample();

        auto ray_direction = pixel_sample - ray_origin;

        // Launch rays in a shutter opening duration
        double ray_time = random_double(0, frame_duration);
        return ray(ray_origin, ray_direction, ray_time);
    }

    // Typical Shadow Ray Direct Lighting (Legacy)
    color shadowray_direct_lighting(const ray &r_in, const hit_info &hit, const scatter_info &sinfo, const hittable &world, const hittable_list &lights, const int ShadowRays = 4) const
    {
        color direct_lighting(0, 0, 0);
        for (int i = 0; i < ShadowRays; ++i)
        {
            // Randomly sample a light source
            auto light = lights.random_hittable();
            vec3 light_dir = light->random(hit.hit_point);

            // Check if the light source is visible
            hit_info light_hit;
            ray shadow_ray(hit.hit_point, light_dir);
            if (world.hit(shadow_ray, interval(0.001, 1), light_hit))
            {
                if (light_hit.hittable_index == light->index)
                {
                    auto scattering_pdf = sinfo.no_pdf ? 1.0 : hit.mat->scattering_pdf(r_in, hit, shadow_ray);
                    auto cosine_theta = dot(hit.normal, normalize(light_dir));
                    auto light_falloff = 10000.0 / squared_distance(hit.hit_point, light_hit.hit_point);
                    color light_radiance = light_falloff * cosine_theta * light_hit.mat->emitter(shadow_ray, light_hit, light_hit.u, light_hit.v, light_hit.hit_point);
                    direct_lighting += sinfo.attenuation * scattering_pdf * light_radiance / light->pdf_value(hit.hit_point, light_dir);
                }
            }
        }
        direct_lighting /= ShadowRays;

        return direct_lighting;
    }

    // RIS Direct Lighting (Shadow rays Resampling)
    color RIS_direct_lighting(const ray &r_in, const hit_info &hit, const scatter_info &sinfo, const hittable &world, const hittable_list &lights, const int RIS_size = 16, const int ShadowRays = 4) const
    {
        struct RIS_sample
        {
            unsigned int light_idx;
            vec3 light_dir;
            color calculated_radiance;
            double target_distribution_val;
            double weight;
        };

        const auto epsilon = 1e-4;

        // Generate RIS participating light samples
        vector<RIS_sample> samples;
        double total_weight = 0;
        for (int i = 0; i < RIS_size; ++i)
        {
            // Randomly sample a light source
            auto light = lights.random_hittable();
            vec3 light_dir = light->random(hit.hit_point);

            // Use lights.pdf as proposal distribution
            auto light_pdf_val = light->pdf_value(hit.hit_point, light_dir);

            // Use function proportional to Li (incoming Radiance) as target distribution
            auto scattering_pdf_val = sinfo.no_pdf ? 1.0 : hit.mat->scattering_pdf(r_in, hit, ray(hit.hit_point, light_dir));
            auto cosine_theta = dot(hit.normal, normalize(light_dir));

            // Clamp
            cosine_theta = cosine_theta > epsilon ? cosine_theta : 0.0;

            hit_info false_hit;
            false_hit.set_face_normal(ray(hit.hit_point, light_dir), light->get_normal(point3()));

            double light_falloff = 10000.0 / squared_length(light_dir);
            color emission = light->get_material()->emitter(ray(hit.hit_point, light_dir), false_hit, false_hit.u, false_hit.v, false_hit.hit_point);
            color light_radiance = light_falloff * cosine_theta * emission;

            // Weight: target_pdf / proposal_pdf
            auto target_distribution_val = squared_length(light_radiance) * scattering_pdf_val;
            auto proposal_distribution_val = light_pdf_val;
            auto weight = target_distribution_val / (proposal_distribution_val + epsilon);

            total_weight += weight;

            samples.push_back({light->index, light_dir, light_radiance, target_distribution_val, weight});
        }

        double average_weight = total_weight / RIS_size;

        color direct_lighting(0, 0, 0);

        // Resampling and solving visibility
        for (int i = 0; i < ShadowRays; ++i)
        {
            // Resample a light source by its weight
            auto xi = random_double(0, total_weight);
            auto sum = 0.0;

            for (const RIS_sample &sample : samples)
            {
                sum += sample.weight;
                if (xi < sum)
                {
                    // Check visibility
                    hit_info light_hit;
                    ray shadow_ray(hit.hit_point, sample.light_dir);
                    if (world.hit(shadow_ray, interval(0.001, 1), light_hit))
                    {
                        if (light_hit.hittable_index == sample.light_idx)
                        {
                            auto scattering_pdf_val = sinfo.no_pdf ? 1.0 : hit.mat->scattering_pdf(r_in, hit, shadow_ray);
                            direct_lighting += sinfo.attenuation * scattering_pdf_val * sample.calculated_radiance / (sample.target_distribution_val + epsilon);
                        }
                    }

                    break;
                }
            }
        }

        return direct_lighting * average_weight / ShadowRays;
    }

    color ray_color(const ray &r, const hittable &world, int current_depth, const hittable_list &lights) const
    {
        hit_info hit;

        // Simply address the floating point error on intersection by ignoring intersecting point which is close enough to surfaces
        if (world.hit(r, interval(0.001, infinity), hit))
        {
            --current_depth;
            if (current_depth > 0)
            {
                color emission = hit.mat->emitter(r, hit, hit.u, hit.v, hit.hit_point);
                scatter_info sinfo;

                if (!hit.mat->scatter(r, hit, sinfo))
                    return emission;

                // if pdf is not available, use specific ray as important ray
                if (sinfo.no_pdf)
                    return sinfo.attenuation * ray_color(sinfo.ray_without_pdf, world, current_depth, lights);

                color direct_lighting;
                if (current_depth == max_depth - 1)
                {
                    // TODO:: For primary rays, use ReSTIR for direct lighting
                    // direct_lighting = shadowray_direct_lighting(r, hit, sinfo, world, lights, shadow_samples);
                    direct_lighting = RIS_direct_lighting(r, hit, sinfo, world, lights, RIS_size, shadow_samples);
                }
                else
                {
                    // For secondary rays, use RIS or just Shadow Ray for direct lighting
                    // Direct lighting (Shadow rays)
                    // direct_lighting = shadowray_direct_lighting(r, hit, sinfo, world, lights, shadow_samples);

                    // Direct lighting (RIS)
                    direct_lighting = RIS_direct_lighting(r, hit, sinfo, world, lights, RIS_size, shadow_samples);
                }

                direct_lighting += emission;

                // Mixture PDF for light sampling and material scattering
                auto lights_pdf = make_shared<hittable_pdf<>>(lights, hit.hit_point);
                mixture_pdf mixed_pdf(vector<double>{0.15, 0.85}, lights_pdf, sinfo.pdf_ptr);

                // auto mixed_pdf = sinfo.pdf_ptr;

                ray scattered = ray(hit.hit_point, mixed_pdf.generate(), r.time());
                auto pdf_val = mixed_pdf.value(scattered.direction());

                double scattering_pdf = hit.mat->scattering_pdf(r, hit, scattered);
                auto cosine_theta = dot(normalize(scattered.direction()), hit.normal);

                color incoming_radiance = cosine_theta * ray_color(scattered, world, current_depth, lights);
                color indirect_lighting = (sinfo.attenuation * scattering_pdf * incoming_radiance) / pdf_val;

                // remap the color to avoid overexposure (Reinhard)
                return (direct_lighting + indirect_lighting) / (1 + direct_lighting + indirect_lighting);

                // debug output for direct lighting
                // return direct_lighting;

                // debug output for indirect lighting
                // return indirect_lighting;
            }

            return color(0, 0, 0);
        }
        // If ray hits nothing, simply return background color
        else
        {
            return background;
        }
    }

    void render_pixel(int i, int j, const hittable &world, const hittable_list &lights, vector<vector<color>> &buffer)
    {
        color pixel_color(0, 0, 0);

        for (int s_i = 0; s_i < sqrt_spp; ++s_i)
        {
            for (int s_j = 0; s_j < sqrt_spp; ++s_j)
            {
                ray r = get_primary_ray(i, j, s_i, s_j);
                pixel_color += ray_color(r, world, max_depth, lights);
            }
        }

        // Write all color into buffer
        buffer[i][j] = pixel_color;
        ++pixel_finished;
    }

    // Indicator for pixel rendering progress
    void pixel_indicator(int total_pixels)
    {
        while (pixel_finished < total_pixels)
        {
            clog << "\rPixels Rendered: " << pixel_finished << " / " << total_pixels << flush;
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }

    // Generate G-buffers for denoising
    void generate_Gbuffers(const hittable &world)
    {
        for (int j = 0; j < image_height; ++j)
        {
            for (int i = 0; i < image_width; ++i)
            {
                ray primary_ray = get_primary_ray(i, j);
                hit_info hit;
                if (world.hit(primary_ray, interval(0.001, infinity), hit))
                {
                    position_buffer.data[i][j] = hit.hit_point;
                    normal_buffer.data[i][j] = hit.normal;
                    index_buffer.data[i][j] = vec3(hit.mat->index, hit.hittable_index, 0);
                }
                else
                {
                    position_buffer.data[i][j] = vec3(0, 0, 0);
                    normal_buffer.data[i][j] = vec3(0, 0, 0);
                    index_buffer.data[i][j] = vec3(0, 0, 0);
                }
            }
        }
    }

    // Generate Reservoirs for ReSTIR
    // void generate_reservoirs(const hittable &world, const hittable_list &lights)
    // {
    //     // Initial Sampling
    //     for (int j = 0; j < image_height; ++j)
    //     {
    //         for (int i = 0; i < image_width; ++i)
    //         {
    //             for (int s_i = 0; s_i < sqrt_spp; ++s_i)
    //             {
    //                 for (int s_j = 0; s_j < sqrt_spp; ++s_j)
    //                 {
    //                     ray r = get_primary_ray(i, j, s_i, s_j);
    //                     hit_info hit;
    //                     if (world.hit(r, interval(0.001, infinity), hit))
    //                     {
    //                         auto pixel_reservoir = reservoir_buffer.data[i][j];
    //                         pixel_reservoir.update(make_shared<hit_info>(hit), 1.0);
    //                     }
    //                 }
    //             }
    //         }
    //     }

    //     // Spatial-Temporal Reuse

    // }
};