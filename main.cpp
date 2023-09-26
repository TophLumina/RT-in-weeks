#include "rtweekend.h"

#include "color.h"
#include "material.h"
#include "camera.h"

int main()
{
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    auto material_left =   make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    auto material_right =  make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_ground));
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0, -1), 0.5, material_left));
    world.add(make_shared<sphere>(point3(1.0, 0, -1), 0.5, material_right));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;
    cam.samplers_per_pixel = 32;
    cam.ray_gen_probability = 0.96;
    clog << "Samplers: " << cam.samplers_per_pixel << '\n';

    cam.render(world);
}