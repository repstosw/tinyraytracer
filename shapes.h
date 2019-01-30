#ifndef SHAPES_H
#define SHAPES_H

#include "geometry.h"

struct Light {
    Vec3f position;
    float intensity;

    Light(const Vec3f& p, float i) : position(p), intensity(i) {}
    Light() : position(), intensity() {}
};

struct Material {
    Vec2f albedo;
    Vec3f diffuse_color;
    float specular;
    

    Material(const Vec2f& a, const Vec3f& color, const float& s) : albedo(a), diffuse_color(color), specular(s) {}
    Material() : albedo(), diffuse_color(), specular() {}
};

struct Sphere {
    Vec3f center;
    float radius;
    Material material;

    Sphere(const Vec3f& c, const float& r, const Material& m) : center(c), radius(r), material(m) {}

    bool ray_intersect(const Vec3f& origin, const Vec3f& direction, float &t0) const {
        // Calculate the vector from origin to center of circle
        Vec3f L = center - origin;

        // Calculate the length along direction vector to projection of sphere's center
        float tca = L * direction;

        // Calculate the distance^2 from projection point to sphere's center
        float d2 = (L * L) - (tca * tca);

        // If distance is greater than radius, the ray never intersects the sphere
        if (d2 > radius * radius) return false;

        // Calculate the distance from projection point on direction vector to spherical intersection point
        float thc = sqrt(radius * radius - d2);

        // Calculate the distance along direction vector to first intersection point
        t0 = tca - thc;

        // Caclulate distance along direction vector from projection point to exit point
        float t1 = tca + thc;

        // If first intersection point is behind ray origin, assign exit point
        if (t0 < 0) t0 = t1;
        
        // If exit point is still behind, we don't intersect sphere
        if (t0 < 0) return false;

        // We intercept at distance t0 along direction vector
        return true;
    }
};

#endif // SHAPES_H
