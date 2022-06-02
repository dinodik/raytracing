#include "utilities.h"
#include "vec3.h"
#include "colour.h"
#include "camera.h"
#include "colour.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"

#include <iostream>

colour ray_colour(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered
    if (depth <= 0)
        return colour(0, 0, 0);

    if (world.hit(r, 0.001, INF, rec)) {
        ray scattered;
        colour attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_colour(scattered, world, depth-1);
        return colour(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * colour(1.0, 1.0, 1.0) + t * colour(0.5, 0.7, 1.0);
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(colour(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 centre(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((centre - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = colour::random() * colour::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(centre, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = colour::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(centre + vec3(0, 0.3, 0), 0.5, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(centre, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(colour(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

int main() {

    // Image

    const auto ASPECT_RATIO = 3.0 / 2.0;
    const int IMAGE_WIDTH = 600;
    const int IMAGE_HEIGHT = static_cast<int>(IMAGE_WIDTH / ASPECT_RATIO);
    const int SAMPLES_PER_PIXEL = 20;
    const int MAX_DEPTH = 20;

    // World

    auto world = random_scene();

    /* auto R = cos(PI/4); */
    /* hittable_list world; */

    /* auto material_ground = make_shared<lambertian>(colour(0.8, 0.8, 0.0)); */
    /* auto material_centre = make_shared<lambertian>(colour(0.1, 0.2, 0.5)); */
    /* auto material_left   = make_shared<dielectric>(1.5); */
    /* auto material_right  = make_shared<metal>(colour(0.8, 0.6, 0.2), 0.0); */

    /* world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground)); */
    /* world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_centre)); */
    /* world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left)); */
    /* world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),  -0.4, material_left)); */
    /* world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right)); */

    // Camera

    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperature = 0.1;

    camera cam(lookfrom, lookat, vup, 20, ASPECT_RATIO, aperature, dist_to_focus);

    // Render

    std::cout << "P3\n" << IMAGE_WIDTH << ' ' << IMAGE_HEIGHT << "\n255\n";

    for (int j = IMAGE_HEIGHT-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < IMAGE_WIDTH; ++i) {
            colour pixel_colour(0, 0, 0);
            for (int s = 0; s < SAMPLES_PER_PIXEL; ++s) {
                auto u = double(i + random_double()) / (IMAGE_WIDTH - 1);
                auto v = double(j + random_double()) / (IMAGE_HEIGHT - 1);
                ray r = cam.get_ray(u, v);
                pixel_colour += ray_colour(r, world, MAX_DEPTH);
            }
            write_colour(std::cout, pixel_colour, SAMPLES_PER_PIXEL);
        }
    }

    std::cerr << "\nDone.\n";
}
