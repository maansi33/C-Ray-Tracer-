#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "spheres.h"
#include "color.h"
#include <math.h>

// Global variables
float lightBrightness;
Vec3 cameraPos = {0.0f, 0.0f, 0.0f};  // camera always at origin
Vec3 lightPos;  // defining light position
Vec3 *colors;
World world; // holds the spheres and objects
Vec3 backgroundColor;  // default black background
float viewportHeight, focalLength;
int imageWidth, imageHeight;
Vec3 intToVec3Color(unsigned int color);


// Function to read input and store data in structs
void readInput(const char *inputPath) {
    FILE *file = fopen(inputPath, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open input file %s\n", inputPath);
        exit(1);
    }

    int backgroundIndex, numSpheres;
    
    // reads image size
    if (fscanf(file, "%d %d", &imageWidth, &imageHeight) != 2) {
       fprintf(stderr, "Error: Invalid image dimensions format\n");
       fclose(file);
       exit(1);
    }
    
    // reads viewport height
    if (fscanf(file, "%f", &viewportHeight) != 1) {
       fprintf(stderr, "Error: Invalid viewport height format\n");
       fclose(file);
       exit(1);
    }
    
    // reads focal length
    if (fscanf(file, "%f", &focalLength) != 1) {
       fprintf(stderr, "Error: Invalid focal length format\n");
       fclose(file);
       exit(1);
    }
    
    // reads light ppts
    if (fscanf(file, "%f %f %f %f", &lightPos.x, &lightPos.y, &lightPos.z, &lightBrightness) != 4) {
        fprintf(stderr, "Error: Invalid light properties format\n");
        fclose(file);
        exit(1);
    }

    // Reads colors 
    int numColors;
    if (fscanf(file, "%d", &numColors) != 1) {
        fprintf(stderr, "Error: Invalid number of colors format\n");
        fclose(file);
        exit(1);
    }
    for (int i = 0; i < numColors; i++) {
        unsigned int color;
        if (fscanf(file, "%x", &color) != 1) {
            fprintf(stderr, "Error: Invalid color format at index %d\n", i);
            fclose(file);
            exit(1);
        }
    }

    // reads background index
    if (fscanf(file, "%d", &backgroundIndex) != 1) {
        fprintf(stderr, "Error: Invalid background index format\n");
        fclose(file);
        exit(1);
    }

    // initialize world and reads the spheres
    worldInit(&world);
    if (fscanf(file, "%d", &numSpheres) != 1) {
        fprintf(stderr, "Error: Invalid sphere count format\n");
        fclose(file);
        exit(1);
    }
    for (int i = 0; i < numSpheres; i++) {
        Vec3 pos;
        float radius;
        int colorIndex; 
        if (fscanf(file, "%f %f %f %f %d", &pos.x, &pos.y, &pos.z, &radius, &colorIndex) != 5) {
            fprintf(stderr, "Error: Invalid sphere data at index %d\n", i);
            continue; 
        }
        addSphere(&world, createSphere(radius, pos, (Vec3){1, 1, 1})); // spheres are white
    }

    fclose(file);
}


#ifdef FS
// Function to read input and store data
void readInputFS(const char *inputPath) {
    FILE *file = fopen(inputPath, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open input file %s\n", inputPath);
        exit(1);
    }

    int backgroundIndex, numSpheres, numColors;

    // read image size
    if (fscanf(file, "%d %d", &imageWidth, &imageHeight) != 2) {
        fprintf(stderr, "Error: Invalid image dimensions format\n");
        fclose(file);
        exit(1);
    }

    // read viewport height
    if (fscanf(file, "%f", &viewportHeight) != 1) {
        fprintf(stderr, "Error: Invalid viewport height format\n");
        fclose(file);
        exit(1);
    }

    // read focal length
    if (fscanf(file, "%f", &focalLength) != 1) {
        fprintf(stderr, "Error: Invalid focal length format\n");
        fclose(file);
        exit(1);
    }

    // read light properties
    if (fscanf(file, "%f %f %f %f", &lightPos.x, &lightPos.y, &lightPos.z, &lightBrightness) != 4) {
        fprintf(stderr, "Error: Invalid light properties format\n");
        fclose(file);
        exit(1);
    }

    // read the number of colors and their hex values
    if (fscanf(file, "%d", &numColors) != 1) {
        fprintf(stderr, "Error: Invalid number of colors format\n");
        fclose(file);
        exit(1);
    }

    // Allocate memory 
    unsigned int *packColors = malloc(numColors * sizeof(unsigned int));  

    // Read each color
    for (int i = 0; i < numColors; i++) {
        if (fscanf(file, "%x", &packColors[i]) != 1) {
            fprintf(stderr, "Error: Invalid color format at index %d\n", i);
            fclose(file);
            exit(1);
        }
    }

    // Sort  colors using qsort 
    qsort(packColors, numColors, sizeof(unsigned int), compareColor);

    // Allocate memory for the colors 
    Vec3 *colorsVec = malloc(numColors * sizeof(Vec3));  // Store Vec3 colors

    // Convert colors to Vec3 colors
    for (int i = 0; i < numColors; i++) {
        colorsVec[i] = intToVec3Color(packColors[i]);  // Convert to Vec3 and store
    }

    // Free the colors array 
    free(packColors);

    // Read the background index and set the background color
    if (fscanf(file, "%d", &backgroundIndex) != 1) {
        fprintf(stderr, "Error: Invalid background index format\n");
        fclose(file);
        exit(1);
    }
    backgroundColor = colorsVec[backgroundIndex];  // set background color 

    // Initialize world and read the spheres
    worldInit(&world);
    if (fscanf(file, "%d", &numSpheres) != 1) {
        fprintf(stderr, "Error: Invalid sphere count format\n");
        fclose(file);
        exit(1);
    }

    // Read each sphere's data 
    for (int i = 0; i < numSpheres; i++) {
        Vec3 pos;
        float radius;
        int colorIndex;
        if (fscanf(file, "%f %f %f %f %d", &pos.x, &pos.y, &pos.z, &radius, &colorIndex) != 5) {
            fprintf(stderr, "Error: Invalid sphere data at index %d\n", i);
            continue;  
        }
        addSphere(&world, createSphere(radius, pos, colorsVec[colorIndex])); 
    }

    
    free(colorsVec);
    fclose(file);
}
#endif


#ifdef MS1
void outputResults(const char *outputPath) {
    FILE *file = fopen(outputPath, "w");
    if (!file) {
        fprintf(stderr, "Error: Could not open output file %s\n", outputPath);
        exit(1);
    }

    // output vector operations
    Vec3 addRes = add(backgroundColor, lightPos);
    Vec3 subRes = subtract(backgroundColor, lightPos);
    Vec3 scalarMulRes = scalarMultiply(2.66666f, lightPos);
    Vec3 normRes = normalize(lightPos);

    // print results of vector operations
    fprintf(file, "(%.1f, %.1f, %.1f) + (%.1f, %.1f, %.1f) = (%.1f, %.1f, %.1f)\n",
            backgroundColor.x, backgroundColor.y, backgroundColor.z,
            lightPos.x, lightPos.y, lightPos.z,
            addRes.x, addRes.y, addRes.z);

    fprintf(file, "(%.1f, %.1f, %.1f) - (%.1f, %.1f, %.1f) = (%.1f, %.1f, %.1f)\n",
            backgroundColor.x, backgroundColor.y, backgroundColor.z,
            lightPos.x, lightPos.y, lightPos.z,
            subRes.x, subRes.y, subRes.z);

    fprintf(file, "2.7 * (%.1f, %.1f, %.1f) = (%.1f, %.1f, %.1f)\n",
            lightPos.x, lightPos.y, lightPos.z,
            scalarMulRes.x, scalarMulRes.y, scalarMulRes.z);
    
    fprintf(file, "normalize(%.1f, %.1f, %.1f) = (%.1f, %.1f, %.1f)\n",
            lightPos.x, lightPos.y, lightPos.z,
            normRes.x, normRes.y, normRes.z);

    // print sphere details 
    for (int i = 0; i < world.size; i++) {
        Sphere *s = world.spheres[i];
        Vec3 divRes = scalarDivide(s->color, s->r);
        float dotRes = dot(lightPos, s->pos);
        float distRes = distance(lightPos, s->pos);
        float lenRes = length(s->pos);

        fprintf(file, "\n(%.1f, %.1f, %.1f) / %.1f = (%.1f, %.1f, %.1f)\n",
                s->color.x, s->color.y, s->color.z, s->r,
                divRes.x, divRes.y, divRes.z);
        fprintf(file, "dot((%.1f, %.1f, %.1f), (%.1f, %.1f, %.1f)) = %.1f\n",
                lightPos.x, lightPos.y, lightPos.z,
                s->pos.x, s->pos.y, s->pos.z,
                dotRes);
        fprintf(file, "distance((%.1f, %.1f, %.1f), (%.1f, %.1f, %.1f)) = %.1f\n",
                lightPos.x, lightPos.y, lightPos.z,
                s->pos.x, s->pos.y, s->pos.z,
                distRes);
        fprintf(file, "length(%.1f, %.1f, %.1f) = %.1f\n",
                s->pos.x, s->pos.y, s->pos.z,
                lenRes);
    }

    fclose(file);
}
#endif

// checks if a point is in shadow
int isInShadow(Vec3 point, Vec3 lightPos, World *world) {
    Vec3 directionToLight = normalize(subtract(lightPos, point));
    float tShadow = 0.0f;

    // casts a ray from the point towards the light
    for (int i = 0; i < world->size; i++) {
        Sphere *sphere = world->spheres[i];
        if (doesIntersect(sphere, point, directionToLight, &tShadow) && tShadow > 0.001f) {
            return 1; // if ray intersects a sphere it is in the shadow
        }
    }

    return 0; // not in shadow
}

// calculates shading with shadows
Vec3 calculateLighting(Vec3 point, Vec3 normal, Vec3 lightPos, Sphere *closestSphere, World *world) {
    Vec3 lightDir = normalize(subtract(lightPos, point));
    float diffuseIntensity = fmax(0.0f, dot(normal, lightDir));

    // Check if point is in shadow
    if (isInShadow(point, lightPos, world)) {
        diffuseIntensity *= 0.1f; // Make it darker if in shadow
    }

    // Use the color of the closest sphere
    return scalarMultiply(diffuseIntensity, closestSphere->color);
}

#ifdef MS2
void render(const char *outputPath) {
    FILE *file = fopen(outputPath, "w");
    if (!file) {
        fprintf(stderr, "Error: Could not open output file %s\n", outputPath);
        exit(1);
    }

    // writes the PPM header
    fprintf(file, "P3\n%d %d\n255\n", imageWidth, imageHeight);  

    // calcualtes the viewport width using the aspect ratio
    float aspectRatio = (float)imageWidth / (float)imageHeight;  
    float viewportWidth = viewportHeight * aspectRatio;  

    // ray tracing loop for each pixel
    for (int j = 0; j < imageHeight; j++) {
      for (int i = 0; i < imageWidth; i++) {
        // converts pixel coordinates to viewport space
        float u = (float)(i - imageWidth / 2) * viewportWidth / imageWidth;
        float v = (float)(imageHeight / 2 - j) * viewportHeight / imageHeight;

        // calculates the direction of the ray
        Vec3 rayDir = (Vec3){u, v, -focalLength}; // z-axis is always -focal length

        // initialize the color and intersection variables
        Vec3 pixelColor = backgroundColor;
        float closestT = INFINITY;
        Sphere *closestSphere = NULL;
        Vec3 intersectionPoint;

        //checks for intersections with all spheres in the world
        for (int k = 0; k < world.size; k++) {
            Sphere *sphere = world.spheres[k];
            float t = 0;
            // if the ray intersects a sphere then update the closest intersection
            if (doesIntersect(sphere, cameraPos, rayDir, &t) && t < closestT) {
                closestT = t;
                intersectionPoint = add(cameraPos, scalarMultiply(t, rayDir));
                closestSphere = sphere;
            }
        }

        // checks for intersection with the ground (at y = 0)
        float tGround = (0.0f - cameraPos.y) / rayDir.y;
        if (tGround > 0.001f && tGround < closestT) {
            intersectionPoint = add(cameraPos, scalarMultiply(tGround, rayDir));
            closestSphere = NULL;  // it is the ground
            closestT = tGround;  // updates closest intersection to the ground
        }

        //if intersection present then calculate the color
        if (closestSphere || closestT < INFINITY) {
            Vec3 normal;
            // calculate the normal at the intersection point
            if (closestSphere) {
                normal = normalize(subtract(intersectionPoint, closestSphere->pos));
            } else {
                normal = (Vec3){0.0f, 1.0f, 0.0f};  // normal for the ground
            }

            // Calculates light direction and intensity
            Vec3 lightDir = normalize(subtract(lightPos, intersectionPoint));
            float distanceToLight = length(subtract(lightPos, intersectionPoint));
            float intensity = lightBrightness * fmaxf(0.0f, dot(lightDir, normal)) / (distanceToLight * distanceToLight);

            // checks if the intersection point is in shadow
            Vec3 shadowRayOrigin = add(intersectionPoint, scalarMultiply(0.001f, normal));   
            int isShadowed = 0;
            // chekcs if any sphere is between the point and source of light
            for (int i = 0; i < world.size; i++) {
                float t;
                if (doesIntersect(world.spheres[i], shadowRayOrigin, lightDir, &t)) {
                    if (t > 0.001f && t < distanceToLight) { 
                        isShadowed = 1;
                        break;
                    }
                }
            }

            // Applies the shadow factor if the point is shadowed
            if (isShadowed) {
                intensity *= 0.1f; // Reduces intensity 
            } 

            // clamp the ntensity and set color
            intensity = fminf(1.0f, fmaxf(0.0f, intensity)); // Clamp between 0 and 1
            pixelColor = (Vec3){intensity, intensity, intensity}; // grayscale
        }

        // outputs  the pixel color
        fprintf(file, "%d %d %d ", (int)(pixelColor.x * 255), (int)(pixelColor.y * 255), (int)(pixelColor.z * 255));
        
        
    }
    fprintf(file, "\n");
  }
  fclose(file);
}
#endif

#ifdef FS
void renderFS(const char *outputPath) {
    FILE *ppmFile = fopen(outputPath, "w");
    if (!ppmFile) {
        fprintf(stderr, "Error: Could not open output file %s\n", outputPath);
        exit(1);
    }

    // Write the PPM header
    fprintf(ppmFile, "P3\n%d %d\n255\n", imageWidth, imageHeight);

    // calculate the viewport width using the aspect ratio
    float aspectRatio = (float)imageWidth / (float)imageHeight;
    float viewportWidth = viewportHeight * aspectRatio;

    // anti-aliasing parameters
    int samplesPerPixel = 9; 
    float sampleStep = 1.0f / sqrtf(samplesPerPixel);

    // Ray tracing loop for each pixel
    for (int j = 0; j < imageHeight; j++) {
        for (int i = 0; i < imageWidth; i++) {
            Vec3 pixelColor = {0.0f, 0.0f, 0.0f};

            // loop over subpixel samples
            for (int s = 0; s < sqrtf(samplesPerPixel); s++) {
                for (int t = 0; t < sqrtf(samplesPerPixel); t++) {
                    // compute subpixel offsets
                    float uOffset = (s + 0.5f) * sampleStep;
                    float vOffset = (t + 0.5f) * sampleStep;

                    // Ccnvert pixel coordinates to viewport space with subpixel offsets
                    float u = ((i + uOffset) - imageWidth / 2.0f) * viewportWidth / imageWidth;
                    float v = ((imageHeight / 2.0f - (j + vOffset)) * viewportHeight / imageHeight);

                    // Calculate the ray direction
                    Vec3 rayDir = {u, v, -focalLength};

                    // Check if the ray intersects any spheres
                    float closestT = INFINITY;
                    Sphere *closestSphere = NULL;
                    Vec3 intersectionPoint;

                    for (int k = 0; k < world.size; k++) {
                        Sphere *sphere = world.spheres[k];
                        float t = 0;
                        if (doesIntersect(sphere, cameraPos, rayDir, &t) && t < closestT) {
                            closestT = t;
                            intersectionPoint = add(cameraPos, scalarMultiply(t, rayDir));
                            closestSphere = sphere;
                        }
                    }

                    // Initialize intensity and normal
                    float intensity = 0.0f;
                    Vec3 normal = {0.0f, 0.0f, 0.0f};

                    if (closestSphere) {
                        // Calculate the normal at the intersection point
                        normal = normalize(subtract(intersectionPoint, closestSphere->pos));

                        // calculate light direction and intensity
                        Vec3 lightDir = normalize(subtract(lightPos, intersectionPoint));
                        float distanceToLight = length(subtract(lightPos, intersectionPoint));
                        intensity = lightBrightness * fmaxf(0.0f, dot(lightDir, normal)) / (distanceToLight * distanceToLight);

                        // Check if the intersection point is in shadow
                        Vec3 shadowRayOrigin = add(intersectionPoint, scalarMultiply(0.01f, normal)); 
                        int isShadowed = 0;

                        for (int k = 0; k < world.size; k++) {
                            float t;
                            if (doesIntersect(world.spheres[k], shadowRayOrigin, lightDir, &t)) {
                                if (t > 0.01f && t < distanceToLight) {
                                    isShadowed = 1;
                                    break;
                                }
                            }
                        }

                        // reduce intensity if shadowed
                        if (isShadowed) {
                            intensity *= 0.1f;
                        }
                        intensity = fminf(1.0f, fmaxf(0.0f, intensity));

                        // set pixel color based on intensity and sphere color
                        Vec3 sphereColor = closestSphere->color;
                        pixelColor = add(pixelColor, scalarMultiply(intensity, sphereColor));
                    } else {
                        // add background color for no intersection
                        pixelColor = add(pixelColor, backgroundColor);
                    }
                }
            }

            // average the results and clamp values
            pixelColor = scalarDivide(pixelColor, (float)(samplesPerPixel));
            pixelColor.x = fminf(1.0f, fmaxf(0.0f, pixelColor.x));
            pixelColor.y = fminf(1.0f, fmaxf(0.0f, pixelColor.y));
            pixelColor.z = fminf(1.0f, fmaxf(0.0f, pixelColor.z));

            // Write the final color
            fprintf(ppmFile, "%d %d %d ",
                    (int)(pixelColor.x * 255),
                    (int)(pixelColor.y * 255),
                    (int)(pixelColor.z * 255));
        }
        fprintf(ppmFile, "\n");
    }

    fclose(ppmFile);
}
#endif



// free the world and its memory
void freeWorld(World *world) {
    for (int i = 0; i < world->size; i++) {
        free(world->spheres[i]);
    }
    free(world->spheres);
    world->spheres = NULL;
}



// main function
int main(int argc, char *argv[]) {
    // checks number of arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }
    

    // reads input data

    const char *inputPath = argv[1];
    
    #ifdef MS1
    readInput(inputPath);
    #endif
    
    #ifdef MS2
    readInput(inputPath);
    #endif
       
    #ifdef FS
    
    readInputFS(inputPath);
    #endif
    
    #ifdef MS1
    const char *outputPath = argv[2]; 
    outputResults(outputPath);
    #endif
    
    #ifdef MS2
    const char *outputPath = argv[2]; 
    render(outputPath); 
    #endif
    
    #ifdef FS
    const char *outputPath = argv[2];
    renderFS(outputPath);
    #endif
   
    
    freeWorld(&world);

    return 0;
}


