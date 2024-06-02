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

auto SceneFunc = cornell_box_modified;

void cornell_box(hittable_list &world, hittable_list &lights, camera &cam)
{
    // Camera settings
    cam.aspect_ratio = 1.0;
    cam.image_width = 800;

    cam.RIS_size = 16;
    cam.shadow_samples = 4;

    cam.vfov = 40;
    cam.lookfrom = point3(27.8, 27.8, -80.0);
    cam.lookat = point3(27.8, 27.8, 0);
    cam.vup = vec3(0, 1, 0);

    cam.max_depth = 4;

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

    cam.RIS_size = 16;
    cam.shadow_samples = 4;
    cam.max_depth = 16;

    cam.vfov = 40;
    cam.lookfrom = point3(27.8, 27.8, -80.0);
    cam.lookat = point3(27.8, 27.8, 0);
    cam.vup = vec3(0, 1, 0);

    auto aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);

    auto glass = make_shared<dielectric>(1.5);

    auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
    auto violet = make_shared<lambertian>(color(0.5, 0.0, 1.0));
    auto chocolate = make_shared<lambertian>(color(0.5, 0.25, 0.0));
    auto orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto azure = make_shared<lambertian>(color(0.0, 0.5, 1.0));

    auto orange_light = make_shared<diffuse_directional_light>(color(9.0, 5.0, 2.0));
    auto azure_light = make_shared<diffuse_directional_light>(color(2.0, 5.0, 9.0));
    auto pink_light = make_shared<diffuse_directional_light>(color(9.0, 2.0, 5.0));
    auto white_light = make_shared<diffuse_directional_light>(color(9.0, 9.0, 9.0));

    world.add(make_shared<quad>(point3(55.5, 0, 0), point3(55.5, 55.5, 0), point3(55.5, 55.5, 55.5), point3(55.5, 0, 55.5), green));
    world.add(make_shared<quad>(point3(0, 0, 0), point3(0, 0, 55.5), point3(0, 55.5, 55.5), point3(0, 55.5, 0), red));
    world.add(make_shared<quad>(point3(0, 0, 0), point3(55.5, 0, 0), point3(55.5, 0, 55.5), point3(0, 0, 55.5), white));
    world.add(make_shared<quad>(point3(55.5, 55.5, 55.5), point3(0, 55.5, 55.5), point3(0, 55.5, 0), point3(55.5, 55.5, 0), white));
    world.add(make_shared<quad>(point3(0, 0, 55.5), point3(55.5, 0, 55.5), point3(55.5, 55.5, 55.5), point3(0, 55.5, 55.5), white));

    auto box1 = cube(point3(0, 0, 0), point3(16.5, 33.0, 16.5), aluminum);
    box1->translate(vec3(26.5, 0, 29.5));
    box1->rotate(vec3(0, 1, 0), degree2radius(15));
    world.add(box1);

    string model_path("C:\\Users\\26257\\Desktop\\RT in weeks\\Assets\\MonkeyHead\\MonkeyHead.fbx");

    auto monkey_head_left = make_shared<Model>(model_path.c_str(), azure);
    monkey_head_left->scale(vec3(8.0));
    monkey_head_left->rotate(vec3(1, 0, 0), degree2radius(-90));
    monkey_head_left->rotate(vec3(0, 1, 0), degree2radius(215));
    monkey_head_left->rotate(vec3(1, 0, 0), degree2radius(35));
    monkey_head_left->translate(vec3(40.0, 8.5, 8.0));
    world.add(std::static_pointer_cast<hittable_list>(monkey_head_left));

    auto monkey_head_right = make_shared<Model>(model_path.c_str(), orange);
    monkey_head_right->scale(vec3(5.5));
    monkey_head_right->rotate(vec3(1, 0, 0), degree2radius(-90));
    monkey_head_right->rotate(vec3(0, 1, 0), degree2radius(160));
    monkey_head_right->rotate(vec3(0, 0, 1), degree2radius(55));
    monkey_head_right->translate(vec3(15.0, 25.0, 25.0));
    world.add(std::static_pointer_cast<hittable_list>(monkey_head_right));

    auto monkey_head_bottom = make_shared<Model>(model_path.c_str(), violet);
    monkey_head_bottom->scale(vec3(2.5));
    monkey_head_bottom->rotate(vec3(1, 0, 0), degree2radius(-90));
    monkey_head_bottom->rotate(vec3(0, 1, 0), degree2radius(155));
    monkey_head_bottom->rotate(vec3(0, 0, 1), degree2radius(20));
    monkey_head_bottom->translate(vec3(30.0, 4.0, 4.5));
    world.add(std::static_pointer_cast<hittable_list>(monkey_head_bottom));

    auto monkey_head_top = make_shared<Model>(model_path.c_str(), chocolate);
    monkey_head_top->scale(vec3(8.0));
    monkey_head_top->rotate(vec3(1, 0, 0), degree2radius(-90));
    monkey_head_top->rotate(vec3(0, 1, 0), degree2radius(185));
    monkey_head_top->rotate(vec3(0, 0, 1), degree2radius(-45));
    monkey_head_top->translate(vec3(34.0, 39.0, 9.0));
    world.add(std::static_pointer_cast<hittable_list>(monkey_head_top));

    auto sphere1 = make_shared<sphere>(point3(19.0, 9.0, 19.0), 9.0, glass);
    auto sphere2 = make_shared<sphere>(point3(19.0, 9.0, 19.0), 8.9, glass);
    world.add(sphere1);
    world.add(sphere2);

    auto light_box1 = cube(point3(-1), point3(1), white_light);
    light_box1->translate(vec3(15, 23, 19));

    auto light_box2 = cube(point3(-1), point3(1), orange_light);
    light_box2->translate(vec3(30, 10, 6));

    auto light_box3 = cube(point3(-1), point3(1), azure_light);
    light_box3->translate(vec3(36, 39, 0));

    auto light_box4 = cube(point3(-1), point3(1), pink_light);
    light_box4->translate(vec3(50, 4, 5));

    auto light_quad = make_shared<quad>(point3(34.3, 55.4, 33.2), point3(34.3, 55.4, 22.7), point3(21.3, 55.4, 22.7), point3(21.3, 55.4, 33.2), white_light);
    world.add(light_quad);
    lights.add(light_quad);

    world.add(light_box1);
    world.add(light_box2);
    world.add(light_box3);
    world.add(light_box4);

    lights.add(light_box1);
    lights.add(light_box2);
    lights.add(light_box3);
    lights.add(light_box4);
}