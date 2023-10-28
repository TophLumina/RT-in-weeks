#include "rtweekend.h"

#include "color.h"
#include "material.h"
#include "camera.h"

#include "BVH.h"

void random_spheres(hittable_list &world);
void two_spheres(hittable_list &world);
void myuvsphere(hittable_list &world);
void two_noise_spheres(hittable_list &world);
void quads(hittable_list &world);

int main(int argc, char const *argv[])
{
    hittable_list world;
    quads(world);

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;
    cam.samplers_per_pixel = argc > 1 ? atoi(argv[argc - 1]) : 8; // samplers
    cam.ray_gen_probability = 0.92;

    cam.vfov = 80;
    cam.lookfrom = point3(0, 0, 9);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);
    cam.background = color(0.7, 0.8, 1.0);

    // cam.defocus_angle = 0.6;
    // cam.focus_dist = 10.0;
    // cam.frame_duration = 1.0;

    clog << "Samplers: " << cam.samplers_per_pixel << '\n';

    cam.render(world);
}

void random_spheres(hittable_list &world)
{
    auto checker = make_shared<checker_texture>(0.32, color(0.1, 0.1, 0.4), color(0.9, 0.9, 0.9));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

    for (int i = -11; i < 11; ++i)
    {
        for (int j = -11; j < 11; ++j)
        {
            auto mat_indicator = random_double();
            point3 center(i + 0.9 * random_double(), 0.2, j + 0.9 * random_double());
            point3 center2 = center;

            if ((center - point3(4, 0.2, 0)).length() > 0.9)
            {
                shared_ptr<material> sphere_material;

                if (mat_indicator < 0.8)
                {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    // center2 = center + vec3(0, random_double(0, .5), 0);
                    sphere_material = make_shared<lambertian>(albedo);
                }
                else if (mat_indicator < 0.95)
                {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                }
                else
                {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                }

                world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));
    world.add(make_shared<sphere>(point3(0, 1, 0), -0.95, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    world = hittable_list(make_shared<bvh_node>(world));
}

void two_spheres(hittable_list &world)
{
    auto checker = make_shared<checker_texture>(0.32, color(0.1, 0.1, 0.4), color(0.9, 0.9, 0.9));

    world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));
}

void myuvsphere(hittable_list &world)
{
    auto earth_texture = make_shared<image_texture>("uvsphere.jpg");
    auto earth_mat = make_shared<lambertian>(earth_texture);

    world.add(make_shared<sphere>(point3(0, 0, 0), 2, earth_mat));
}

void two_noise_spheres(hittable_list &world)
{
    auto perlin_texture = make_shared<noise_texture>(4);

    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(perlin_texture)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(perlin_texture)));
}

void quads(hittable_list &world)
{
    // Materials
    auto left_red =     make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green =   make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue =   make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal =   make_shared<lambertian>(color(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quad>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));
}