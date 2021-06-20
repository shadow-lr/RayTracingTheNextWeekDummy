#include "ray.h"
#include "vec3.h"
#include "rtweekend.h"

#include "moving_sphere.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"

#include "camera.h"

#include <iostream>
#include <thread>
#include <mutex>

// function
hittable_list random_scene();

// Image
const double aspect_ratio = 16.0 / 9.0;
const int image_width = 1600;
const int image_height = static_cast<int>(image_width / aspect_ratio);
const int max_depth = 50;
const int samples_per_pixel = 60;

// World
hittable_list world = random_scene();

// Camera
point3 lookfrom(13, 2, 3);
point3 lookat(0, 0, 0);
vec3 vup(0, 1, 0);
auto dist_to_focus = 10.0;
auto aperture = 0.1;

camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

std::vector<std::vector<color>> color_table(image_height + 1, std::vector<color>(image_width + 1));

// ray recursion
color ray_color(const ray &r, const hittable &world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    // 几何体的颜色
    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;

        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }

    // 背景色
    vec3 unit_direction = unit_vector(r.direction());
    // x y z 映射 r g b
    double t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

void scan_calculate_color(int height, int width) {
    int i = width, j = height;
    color pixel_color(0, 0, 0);
    for (int s = 0; s < samples_per_pixel; ++s) {
        double u = (i + random_double()) / (image_width - 1.0);
        double v = (j + random_double()) / (image_height - 1.0);
        ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, world, max_depth);
    }
//    write_color(std::cout, pixel_color, samples_per_pixel);
    write_color_table(pixel_color, samples_per_pixel, color_table, j, i);
}

hittable_list random_scene() {
    hittable_list world;

    // 原来的地面
//    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
//    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    auto checker = make_shared<checker_texture>(color(0.2,0.3,0.1),color(0.9,0.9,0.9));
    world.add(make_shared<sphere>(point3(0,-1000,0),1000,make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0, 0.5), 0);
                    world.add(make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

int main() {
    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\routput remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            scan_calculate_color(j, i);
        }
    }

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\routput remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            out_color_table(std::cout, color_table, j, i);
        }
    }

    std::cerr << "\nDone.\n";
}