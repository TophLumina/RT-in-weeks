#include "camera.h"
#include "BVH.h"
#include "hittable_list.h"
#include "scenelib.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    hittable_list world;
    hittable_list lights;
    camera cam;

    SceneFunc(world, lights, cam);

    // Building acceleration structure(BVH Tree)
    world = hittable_list(make_shared<bvh_node>(world));
    lights = hittable_list(make_shared<bvh_node>(lights));

    cam.samplers_per_pixel = argc > 1 ? atoi(argv[argc - 1]) : 64; // samplers per pixel

    // cam.defocus_angle = 0.6;
    // cam.focus_dist = 10.0;
    // cam.frame_duration = 1.0;
    clog << "Samplers: " << cam.samplers_per_pixel << '\n';

    cam.render(world, lights);
}