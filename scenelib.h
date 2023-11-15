#include "rtweekend.h"

#include "color.h"
#include "material.h"

#include "sphere.h"
#include "quad.h"
#include "constant_medium.h"

#include "BVH.h"

void random_spheres(hittable_list &world);
void two_spheres(hittable_list &world);
void myuvsphere(hittable_list &world);
void two_noise_spheres_with_lighting(hittable_list &world);
void quads(hittable_list &world);
void cornell_box(hittable_list &world);
void smoke_cornell(hittable_list &world);
void all_features(hittable_list &world);

auto SceneFunc = all_features;

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

void two_noise_spheres_with_lighting(hittable_list &world)
{
    auto perlin_texture = make_shared<noise_texture>(4);

    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(perlin_texture)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(perlin_texture)));

    auto diff_light = make_shared<diffuse_light>(color(1, 1, 1));
    world.add(make_shared<quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), diff_light));
    world.add(make_shared<sphere>(point3(0, 7, 0), 2, diff_light));
}

void quads(hittable_list &world)
{
    // Materials
    auto left_red = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal = make_shared<lambertian>(color(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quad>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));
}

void cornell_box(hittable_list &world)
{
    auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = cube(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    world.add(box1);

    shared_ptr<hittable> box2 = cube(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    world.add(box2);
}

void smoke_cornell(hittable_list &world)
{
    auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = cube(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<hittable> box2 = cube(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    world.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
    world.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));
}

void all_features(hittable_list &world)
{
    hittable_list boxes_ground;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int box_per_side = 20;
    for (int i = 0; i < box_per_side; ++i)
        for (int j = 0; j < box_per_side; ++j)
        {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;

            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes_ground.add(cube(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }

    world.add(make_shared<hittable_list>(boxes_ground));

    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    world.add(make_shared<quad>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto sphere_mat = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, sphere_mat));

    world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)));

    auto mist_boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    world.add(mist_boundary);
    world.add(make_shared<constant_medium>(mist_boundary, 0.2, color(0.2, 0.4, 0.9)));
    mist_boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(mist_boundary, 1e-4, color(1, 1, 1)));

    auto earth_mat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    world.add(make_shared<sphere>(point3(400, 200, 400), 100, earth_mat));

    auto perlin_tex = make_shared<noise_texture>(0.1);
    world.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(perlin_tex)));

    hittable_list boxes_cube;
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    int n = 1000;
    for (int i = 0; i < n; ++i)
        boxes_cube.add(make_shared<sphere>(point3::random(0, 165), 10, white));

    world.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<hittable_list>(boxes_cube), 15),
        vec3(-100, 270, 395)));
}