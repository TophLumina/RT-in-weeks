#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "rtweekend.h"
#include "sphere.h"
#include "vec.h"
#include <memory>


void cornell_box(hittable_list &world, hittable_list &lights);
void cornell_box_modified(hittable_list &world, hittable_list &lights);

auto SceneFunc = cornell_box;

void cornell_box(hittable_list &world, hittable_list &lights)
{
    auto aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.04);
    auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
    auto light = make_shared<diffuse_directional_light>(color(15, 15, 15));

    world.add(make_shared<quad>(point3(555, 0, 0), point3(555, 555, 0), point3(555, 555, 555), point3(555, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), point3(0, 0, 555), point3(0, 555, 555), point3(0, 555, 0), red));
    world.add(make_shared<quad>(point3(343, 554, 332), point3(343, 554, 227), point3(213, 554, 227), point3(213, 554, 332), light));
    world.add(make_shared<quad>(point3(0, 0, 0), point3(555, 0, 0), point3(555, 0, 555), point3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), point3(0, 555, 555), point3(0, 555, 0), point3(555, 555, 0), white));
    world.add(make_shared<quad>(point3(0, 0, 555), point3(555, 0, 555), point3(555, 555, 555), point3(0, 555, 555), white));

    auto box1 = cube(point3(0, 0, 0), point3(165, 330, 165), white);
    // box1->scale(vec3(0.5, 1, 0.5));
    box1->translate(vec3(265, 0, 295));
    box1->rotate(vec3(0, 1, 0), degree2radius(15));
    world.add(box1);

    auto box2 = cube(point3(0, 0, 0), point3(165, 165, 165), white);
    // box2->scale(vec3(0.5, 1, 0.5));
    box2->translate(vec3(130, 0, 65));
    box2->rotate(vec3(0, 1, 0), degree2radius(-18));
    world.add(box2);

    auto m = shared_ptr<material>();
    lights.add(make_shared<quad>(point3(343, 554, 332), point3(343, 554, 227), point3(213, 554, 227), point3(213, 554, 332), m));
}

void cornell_box_modified(hittable_list &world, hittable_list &lights)
{
    auto aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    auto glass = make_shared<dielectric>(1.5);
    auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
    auto light = make_shared<diffuse_directional_light>(color(15, 15, 15));

    world.add(make_shared<quad>(point3(555, 0, 0), point3(555, 555, 0), point3(555, 555, 555), point3(555, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), point3(0, 0, 555), point3(0, 555, 555), point3(0, 555, 0), red));
    world.add(make_shared<quad>(point3(343, 554, 332), point3(343, 554, 227), point3(213, 554, 227), point3(213, 554, 332), light));
    world.add(make_shared<quad>(point3(0, 0, 0), point3(555, 0, 0), point3(555, 0, 555), point3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), point3(0, 555, 555), point3(0, 555, 0), point3(555, 555, 0), white));
    world.add(make_shared<quad>(point3(0, 0, 555), point3(555, 0, 555), point3(555, 555, 555), point3(0, 555, 555), white));

    auto box1 = cube(point3(0, 0, 0), point3(165, 330, 165), aluminum);
    box1->translate(vec3(265, 0, 295));
    box1->rotate(vec3(0, 1, 0), degree2radius(15));
    world.add(box1);

    auto sphere1 = make_shared<sphere>(point3(190, 90, 190), 90.0, glass);
    world.add(sphere1);

    auto m = shared_ptr<material>();
    lights.add(make_shared<quad>(point3(343, 554, 332), point3(343, 554, 227), point3(213, 554, 227), point3(213, 554, 332), m));
}