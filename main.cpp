#include "rtweekend.h"

#include "camera.h"

#include "BVH.h"

#include "scenelib.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    hittable_list world;
    hittable_list lights;
    SceneFunc(world, lights);

    // Building acceleration structure(BVH Tree)
    world = hittable_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 800;
    cam.samplers_per_pixel = argc > 1 ? atoi(argv[argc - 1]) : 8; // samplers
    cam.max_depth = 50;

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    // cam.defocus_angle = 0.6;
    // cam.focus_dist = 10.0;
    // cam.frame_duration = 1.0;
    clog << "Samplers: " << cam.samplers_per_pixel << '\n';

    cam.render(world, lights);

    // Debugging
    // DisneyBRDF brdf(color(1, 1, 1), 0.0, 0.5);
    // vec3 wi = normalize(vec3(0, 0, 1));
    // vec3 n = vec3(0, 0, 1);
    // for (int i = 0; i < 64; ++i)
    // {
    //     try
    //     {
    //         vec3 wo = random_hemisphere_surface(n, 1.0);
    //         color c = brdf(wi, wo, n) * dot(wo, n);
    //         vec h = normalize(wi + wo);
    //         double cos_theta = dot(h, n);
    //         clog << "cos_theta: " << cos_theta << "color: " << c << '\n';
    //     }
    //     catch (const std::exception &e)
    //     {
    //         clog << e.what() << '\n';
    //     }
    // }

    // GGX_pdf pdf(vec3(0, 0, 1), 1.0);
    // for (int i = 0; i < 64; ++i)
    // {
    //     try
    //     {
    //         vec3 wo = pdf.generate();
    //         double pdf_val = pdf.value(wo);
    //         clog << "pdf_val: " << pdf_val << "wo: " << wo << '\n';
    //     }
    //     catch (const std::exception &e)
    //     {
    //         clog << e.what() << '\n';
    //     }
    // }
}