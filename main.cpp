#include "rtweekend.h"

#include "color.h"
#include "material.h"
#include "camera.h"

#include "BVH.h"

void random_spheres(hittable_list &world);
void two_spheres(hittable_list &world);

int main()
{
    hittable_list world;
    random_spheres(world);

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1200;
    cam.samplers_per_pixel = 8;
    cam.ray_gen_probability = 0.92;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;
    cam.frame_duration = 1.0;

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
    world.add(make_shared<sphere>(point3(0,  10, 0), 10, make_shared<lambertian>(checker)));
}