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
const int fov = 90;

std::vector<Sphere> spheres;

void write_png(std::vector<Vec3f>& buffer) {
    auto data = new uint8_t[width * height * 3];

    int j = 0;
    for (auto const& pixel: buffer) {
        data[j] = pixel.x * 255;
        data[j+1] = pixel.y * 255;
        data[j+2] = pixel.z * 255;
        j += 3;
    }

    stbi_write_png("test.png", width, height, 3, data, width * 3);
}

Vec3f cast_ray(const Vec3f &origin, const Vec3f& direction, const Sphere& sphere) {

    float sphere_dist = std::numeric_limits<float>::max();
    if (!sphere.ray_intersect(origin, direction, sphere_dist)) {
        return Vec3f(0.2, 0.7, 0.8);
    }
    return Vec3f(0.4, 0.4, 0.3);
}

// Get a normalized direction vector for a given screen coordinate
Vec3f getDirection(int x, int y) {
    float xf = (2 * (x + 0.5) / (float) width - 1) * tan(fov / 2.0) * width / (float) height;
    float yf = (2 * (y + 0.5) / (float) height - 1) * tan(fov / 2.0);
    return Vec3f(xf, yf, -1).normalize();
}

void render() {

    std::vector<Vec3f> framebuffer(width * height);

    Sphere sphere = Sphere(Vec3f(1, 1, -5), 2.0f);

    Vec3f cameraPosition = Vec3f(0, 0, 0);

    for (size_t j = 0; j < height; j++) {
        for (size_t i = 0; i < width; i++) {
            Vec3f direction = getDirection(i, j);

            framebuffer[i + j * width] = cast_ray(cameraPosition, direction, sphere);
        }
    }

    write_png(framebuffer);

}

int main() {
    render();
    return 0;
}
                                            
