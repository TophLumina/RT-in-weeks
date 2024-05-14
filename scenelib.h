#include "Model.h"
#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "rtweekend.h"
#include "sphere.h"
#include "vec.h"

#include <memory>

void model_test(hittable_list &world, hittable_list &lights, camera &cam);
void cornell_box(hittable_list &world, hittable_list &lights, camera &cam);
void cornell_box_modified(hittable_list &world, hittable_list &lights, camera &cam);

auto SceneFunc = cornell_box;

void model_test(hittable_list &world, hittable_list &lights, camera &cam)
{
    // Camera settings
    cam.aspect_ratio = 1.0;
    cam.image_width = 800;

    cam.vfov = 40;
    cam.lookfrom = point3(100, 100, -150);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.background = color(0.5);

    auto light = make_shared<diffuse_directional_light>(color(15));
    auto light_quad = make_shared<quad>(point3(343, 554, 332), point3(343, 554, 227), point3(213, 554, 227), point3(213, 554, 332), light);
    world.add(light_quad);

    // shared_ptr<Model> model = make_shared<Model>("./Assets/CornellBox-Original/CornellBox-Original.obj");
    shared_ptr<Model> model = make_shared<Model>("C:/Users/26257/Desktop/RT in weeks/Assets/ModelUsedforGraphics/ModelforGraphics.fbx");
    model->scale(vec3(35));
    model->rotate(vec3(1, 0, 0), degree2radius(-90));
    model->rotate(vec3(0, 1, 0), degree2radius(90));
    world.add(std::static_pointer_cast<hittable_list>(model));

    lights.add(light_quad);
}

void cornell_box(hittable_list &world, hittable_list &lights, camera &cam)
{
    // Camera settings
    cam.aspect_ratio = 1.0;
    cam.image_width = 800;

    cam.shadow_samples = 1;

    cam.vfov = 40;
    cam.lookfrom = point3(27.8, 27.8, -80.0);
    cam.lookat = point3(27.8, 27.8, 0);
    cam.vup = vec3(0, 1, 0);

    auto aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.04);
    auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
    auto light = make_shared<diffuse_directional_light>(color(15));
    auto light_quad = make_shared<quad>(point3(34.3, 55.4, 33.2), point3(34.3, 55.4, 22.7), point3(21.3, 55.4, 22.7), point3(21.3, 55.4, 33.2), light);

    world.add(make_shared<quad>(point3(55.5, 0, 0), point3(55.5, 55.5, 0), point3(55.5, 55.5, 55.5), point3(55.5, 0, 55.5), green));
    world.add(make_shared<quad>(point3(0, 0, 0), point3(0, 0, 55.5), point3(0, 55.5, 55.5), point3(0, 55.5, 0), red));
    world.add(light_quad);
    world.add(make_shared<quad>(point3(0, 0, 0), point3(55.5, 0, 0), point3(55.5, 0, 55.5), point3(0, 0, 55.5), white));
    world.add(make_shared<quad>(point3(55.5, 55.5, 55.5), point3(0, 55.5, 55.5), point3(0, 55.5, 0), point3(55.5, 55.5, 0), white));
    world.add(make_shared<quad>(point3(0, 0, 55.5), point3(55.5, 0, 55.5), point3(55.5, 55.5, 55.5), point3(0, 55.5, 55.5), white));

    auto box1 = cube(point3(0, 0, 0), point3(16.5, 33.0, 16.5), white);
    // box1->scale(vec3(0.5, 1, 0.5));
    box1->translate(vec3(26.5, 0, 29.5));
    box1->rotate(vec3(0, 1, 0), degree2radius(15));
    world.add(box1);

    auto box2 = cube(point3(0, 0, 0), point3(16.5, 16.5, 16.5), white);
    // box2->scale(vec3(0.5, 1, 0.5));
    box2->translate(vec3(13.0, 0, 6.5));
    box2->rotate(vec3(0, 1, 0), degree2radius(-18));
    world.add(box2);

    lights.add(light_quad);
}

void cornell_box_modified(hittable_list &world, hittable_list &lights, camera &cam)
{
    // Camera settings
    cam.aspect_ratio = 1.0;
    cam.image_width = 800;

    cam.shadow_samples = 1;

    cam.vfov = 40;
    cam.lookfrom = point3(27.8, 27.8, -80.0);
    cam.lookat = point3(27.8, 27.8, 0);
    cam.vup = vec3(0, 1, 0);

    auto aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    auto glass = make_shared<dielectric>(1.5);
    auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
    auto light = make_shared<diffuse_directional_light>(color(15));
    auto light_quad = make_shared<quad>(point3(34.3, 55.4, 33.2), point3(34.3, 55.4, 22.7), point3(21.3, 55.4, 22.7), point3(21.3, 55.4, 33.2), light);

    world.add(make_shared<quad>(point3(55.5, 0, 0), point3(55.5, 55.5, 0), point3(55.5, 55.5, 55.5), point3(55.5, 0, 55.5), green));
    world.add(make_shared<quad>(point3(0, 0, 0), point3(0, 0, 55.5), point3(0, 55.5, 55.5), point3(0, 55.5, 0), red));
    world.add(light_quad);
    world.add(make_shared<quad>(point3(0, 0, 0), point3(55.5, 0, 0), point3(55.5, 0, 55.5), point3(0, 0, 55.5), white));
    world.add(make_shared<quad>(point3(55.5, 55.5, 55.5), point3(0, 55.5, 55.5), point3(0, 55.5, 0), point3(55.5, 55.5, 0), white));
    world.add(make_shared<quad>(point3(0, 0, 55.5), point3(55.5, 0, 55.5), point3(55.5, 55.5, 55.5), point3(0, 55.5, 55.5), white));

    auto box1 = cube(point3(0, 0, 0), point3(16.5, 33.0, 16.5), aluminum);
    box1->translate(vec3(26.5, 0, 29.5));
    box1->rotate(vec3(0, 1, 0), degree2radius(15));
    world.add(box1);

    auto sphere1 = make_shared<sphere>(point3(19.0, 9.0, 19.0), 9.0, glass);
    world.add(sphere1);

    lights.add(light_quad);
}