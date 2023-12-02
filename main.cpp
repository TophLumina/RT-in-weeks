#include "rtweekend.h"

#include "color.h"
#include "material.h"
#include "camera.h"

#include "BVH.h"

#include "scenelib.h"

// int main(int argc, char const *argv[])
// {
//     hittable_list world;
//     SceneFunc(world);

//     // Building acceleration structure(BVH Tree)
//     world = hittable_list(make_shared<bvh_node>(world));

//     camera cam;

//     cam.aspect_ratio = 1.0;
//     cam.image_width = 800;
//     cam.samplers_per_pixel = argc > 1 ? atoi(argv[argc - 1]) : 8; // samplers
//     cam.ray_gen_probability = 0.96;

//     cam.vfov = 40;
//     cam.lookfrom = point3(278, 278, -800);
//     cam.lookat = point3(278, 278, 0);
//     cam.vup = vec3(0, 1, 0);

//     // cam.defocus_angle = 0.6;
//     // cam.focus_dist = 10.0;
//     // cam.frame_duration = 1.0;
//     clog << "Samplers: " << cam.samplers_per_pixel << '\n';

//     cam.render(world);
// }

#include "MonteCarlo.h"

int main(int argc, char const *argv[])
{
    show(10000);
}
