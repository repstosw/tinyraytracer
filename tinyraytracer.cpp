#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry.h"
#include "shapes.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const int width = 1024;
const int height = 768;
const float fov = M_PI / 2.0;

std::vector<Light> lights;

void write_png(std::vector<Vec3f>& buffer) {
    auto data = new uint8_t[width * height * 3];

    int j = 0;
    for (auto & pixel: buffer) {
        float max = std::max(pixel.x, std::max(pixel.y, pixel.z));
        if (max > 1) pixel = pixel * (1.0 / max);
        data[j] = (uint8_t) 255 * std::max(0.0f, std::min(1.0f, pixel.x));
        data[j+1] = (uint8_t) 255 * std::max(0.0f, std::min(1.0f, pixel.y));
        data[j+2] = (uint8_t) 255 * std::max(0.0f, std::min(1.0f, pixel.z));
        j += 3;
    }

    stbi_write_png("test.png", width, height, 3, data, width * 3);
}

bool scene_intersect(const Vec3f& origin, const Vec3f& direction, const std::vector<Sphere>& spheres,
        Vec3f& hit, Vec3f& N, Material& material) {
    
    float scene_dist = std::numeric_limits<float>::max();

    for (auto const& sphere: spheres) {
        float dist_i;
        if (sphere.ray_intersect(origin, direction, dist_i) && dist_i < scene_dist) {
            scene_dist = dist_i;

            // Get the closest hit coordinate
            hit = origin + direction * dist_i;

            // Get the sphere normal at hit coordinate
            N = (hit - sphere.center).normalize();

            material = sphere.material;
        }
    }
    return scene_dist < 1000;
}


Vec3f cast_ray(const Vec3f &origin, const Vec3f& direction, const std::vector<Sphere> spheres) {
    Vec3f point, N;
    Material material;

    if (!scene_intersect(origin, direction, spheres, point, N, material)) {
            return Vec3f(0.2, 0.2, 0.4);
    }

    // Calculate lighting
    float diffuse_light_intensity = 0;
    float specular_light_intensity = 0;
    for (auto const& light: lights) {
        // Calculate the direction of the light from the hit point
        Vec3f light_direction = light.position - point;
        // Get the distance between point and light
        float light_distance = light_direction.norm();
        // Normalize
        light_direction.normalize();

        // Offset the hit point towards the light along the normal
        Vec3f shadow_origin = (light_direction * N < 0) ? point - (N * 1e-3) : point + (N * 1e-3);
        Vec3f shadow_point, shadow_N;
        Material notused;

        // Don't do lighting calculations for this light, if a) there's a sphere somewhere along the direction
        // towards the light, and b) that sphere is closer than the light itself
        if (scene_intersect(shadow_origin, light_direction, spheres, shadow_point, shadow_N, notused)
                && (shadow_point - shadow_origin).norm() < light_distance) {
            continue;
        }

        // Add in diffuse intensity for this light, proportional to the angle between the geometry normal and
        // the direction to the light 
        diffuse_light_intensity += light.intensity * std::max(0.0f, light_direction * N); 

        // Reflect the light direction around the normal
        Vec3f R = ((2.0f * (-light_direction * N) * N) + light_direction).normalize();


        // Compute specular highlight
        specular_light_intensity += light.intensity * std::powf(std::max(0.0f, R * direction), material.specular);
    }
    return material.diffuse_color * (diffuse_light_intensity * material.albedo[0]) + 
            (Vec3f(1.0, 1.0, 1.0) * specular_light_intensity * material.albedo[0]);
}


// Get a normalized direction vector for a given screen coordinate
Vec3f getDirection(int x, int y) {
    float xf = (2 * (x + 0.5) / (float) width - 1) * tan(fov / 2.0) * width / (float) height;
    float yf = (2 * (y + 0.5) / (float) height - 1) * tan(fov / 2.0);
    return Vec3f(xf, yf, -1).normalize();
}

void render() {
    lights.push_back(Light(Vec3f(-6, 8, 0), 1));
    lights.push_back(Light(Vec3f(10, -10, -15), 5));

    Material red_velvet(Vec2f(0.6, 0.3), Vec3f(0.8, 0.2, 0.0), 5); 
    Material white(Vec2f(0.9, 0.1), Vec3f(0.4, 0.4, 0.3), 50);

    std::vector<Sphere> spheres;

    spheres.push_back(Sphere(Vec3f(3, 0, -16), 2.0f, white));
    spheres.push_back(Sphere(Vec3f(-1, 1.5, -12), 2.0f, white));
    spheres.push_back(Sphere(Vec3f(-6, -4, -15), 4.0f, red_velvet));
    spheres.push_back(Sphere(Vec3f(4, -3, -20), 2.0f, red_velvet));
    spheres.push_back(Sphere(Vec3f(-7, 8.5, -12), 5.0f, red_velvet));

    Vec3f cameraPosition = Vec3f(0, 0, 0);

    std::vector<Vec3f> framebuffer(width * height);

    for (size_t j = 0; j < height; j++) {
        for (size_t i = 0; i < width; i++) {
            Vec3f direction = getDirection(i, j);

            framebuffer[i + j * width] = cast_ray(cameraPosition, direction, spheres);
        }
    }

    write_png(framebuffer);

}

int main() {
    render();
    return 0;
}
                                            
