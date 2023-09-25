#include "rtweekend.h"

#include "camera.h"

int main()
{
    hittable_list world;

    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1600;
    cam.samplers_per_pixel = 64;
    // cam.ray_gen_possibility = 0.96;

    cam.render(world);
}