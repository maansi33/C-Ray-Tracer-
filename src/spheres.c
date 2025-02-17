#include <stdio.h>
#include <stdlib.h>
#include "spheres.h"
#include <math.h>
#include "vector.h"  

Vec3 rayPos; 
Vec3 rayDir; 
Vec3 spherePos; 
float sphereRadius; 

// initializes the world
void worldInit(World *world) {
    world->size = 0;
    world->capacity = 10; 
    world->spheres = (Sphere **)malloc(sizeof(Sphere *) * world->capacity);
    if (!world->spheres) {
        fprintf(stderr, "Memory allocation failed for world->spheres\n");
        exit(1);
    }
}


int doesIntersect(const Sphere *sphere, Vec3 rayPos, Vec3 rayDir, float *t) {
    // calculate the vector from the ray's position to the sphere's center
    Vec3 V = subtract(rayPos, sphere->pos);

    // calculate the coefficients for the quadratic equation
    float a = dot(rayDir, rayDir);
    float b = 2.0f * dot(rayDir, V);
    float c = dot(V, V) - sphere->r * sphere->r;

    // calculate discriminant
    float discriminant = b * b - 4 * a * c;
    
    // if the discriminant is negative, no intersection
    if (discriminant < 0) {
        return 0; 
    }

    // calculate the square root of the discriminant
    float sqrtDiscriminant = sqrtf(discriminant);

    // compute the two possible intersection points
    float t1 = (-b - sqrtDiscriminant) / (2 * a);
    float t2 = (-b + sqrtDiscriminant) / (2 * a);

    // if both t1 and t2 are negative, the sphere is behind the ray
    if (t1 < 0 && t2 < 0) {
        return 0;
    }

    // choose the smallest positive t 
    if (t1 > 0 && t2 > 0) {
        *t = fminf(t1, t2);
    } else {
        *t = fmaxf(t1, t2); 
    }
    
    return 1; // intersection  is found
}



// creates a sphere
Sphere *createSphere(float radius, Vec3 position, Vec3 color) {
    Sphere *sphere = (Sphere *)malloc(sizeof(Sphere));
    if (!sphere) {
        fprintf(stderr, "Memory allocation failed for sphere\n");
        return NULL;
    }
    sphere->r = radius;
    sphere->pos = position;
    sphere->color = color;
    return sphere;
}

// adds a sphere
void addSphere(World *world, Sphere *sphere) {
    if (world->size >= world->capacity) {
        world->capacity *= 2;
        Sphere **new_spheres = (Sphere **)realloc(world->spheres, sizeof(Sphere *) * world->capacity);
        if (!new_spheres) {
            fprintf(stderr, "Reallocation failed for world->spheres\n");
            exit(1);
        }
        world->spheres = new_spheres;
    }
    world->spheres[world->size++] = sphere;
}

void addSpheresFromInput(World *world, FILE *inputFile) {
    int width, height, sphereCount;
    float viewportWidth;

    // read res and viewport width
    if (fscanf(inputFile, "%d %d %f", &width, &height, &viewportWidth) != 3) {
        fprintf(stderr, "Invalid resolution or viewport data\n");
        return;
    }

    // skip extra data
    char skipLine[256]; 
    for (int i = 0; i < 3; i++) {
        if (!fgets(skipLine, sizeof(skipLine), inputFile)) {
            fprintf(stderr, "Unexpected EOF while skipping lines\n");
            return;
        }
    }

    // read sphere count
    if (fscanf(inputFile, "%d", &sphereCount) != 1 || sphereCount <= 0) {
        fprintf(stderr, "Invalid sphere count\n");
        return;
    }

    // allocate memory for spheres
    if (sphereCount > world->capacity) {
        Sphere **new_spheres = realloc(world->spheres, sizeof(Sphere *) * sphereCount);
        if (!new_spheres) {
            fprintf(stderr, "Reallocation failed for world->spheres\n");
            exit(1);
        }
        world->spheres = new_spheres;
        world->capacity = sphereCount;
    }

    // read spheres
    for (int i = 0; i < sphereCount; i++) {
        float x, y, z, r;
        if (fscanf(inputFile, "%f %f %f %f", &x, &y, &z, &r) != 4 || r <= 0) {
            fprintf(stderr, "Invalid sphere data at index %d\n", i);
            continue;
        }
        Vec3 position = {x, y, z};
        Vec3 color = {1.0f, 1.0f, 1.0f}; 
        Sphere *sphere = createSphere(r, position, color);
        if (sphere) {
            addSphere(world, sphere);
        }
    }
}
