#include "color.h"
#include <stdlib.h>   
#include <stdio.h>
#include "vector.h"
#include "spheres.h"
#include <math.h>     


// converts unsigned int color to Vec3
Vec3 intToVec3Color(unsigned int color) {
    Vec3 vec;
    vec.x = ((color >> 16) & 0xFF) / 255.0f;  // Red
    vec.y = ((color >> 8) & 0xFF) / 255.0f;   // Green
    vec.z = (color & 0xFF) / 255.0f;          // Blue
    return vec;
}
// for sorting the colours
int compareColor(const void *a, const void *b)
{
    int a1 = 0, b1 = 0;
    // combines every byte into an int
    for (int i = 0; i < sizeof(int); i++)
    {
        a1 |= (*((unsigned char*)a + i) & 0x0F) << (i * 8);
        b1 |= (*((unsigned char*)b + i) & 0x0F) << (i * 8);
    }
    // compare colours by their int value
    return (a1 < b1) ? -1 : (b1 < a1) ? 1 : (*((int*)a) < *((int*)b)) ? -1 : (*((int*)a) > *((int*)b)) ? 1 : 0;
}

// Global variables
unsigned int backgroundColor;
World world;  



// compute intensity of lighting at a point
float computeIntensity(Vec3 lightPos, Vec3 intersectionPoint, Vec3 normal, float brightness) {
    // calcualtes direction from intersection point to light source
    Vec3 lightDir = subtract(lightPos, intersectionPoint);
    lightDir = normalize(lightDir);   // normalize light direction
    
    // dot product b/w light direction and normal
    float dotProduct = fmax(0.0f, dot(lightDir, normal));  
    
    // compjutes the intensity based on brightness andthe distance
    float intensity = fmin(1.0f, brightness * dotProduct / distance2(lightPos, intersectionPoint));

    
    return intensity;
}

// function to apply lighting to a sphere color
Vec3 applyLighting(Vec3 sphereColor, float intensity) {
    Vec3 finalColor;
    finalColor.x = sphereColor.x * intensity;
    finalColor.y = sphereColor.y * intensity;
    finalColor.z = sphereColor.z * intensity;
    
    return finalColor;
}

//calculates the final pixel color after lighting
void calculatePixelColor(Vec3 lightPos, float lightBrightness, Vec3 intersectionPoint, Vec3 normal, Vec3 sphereColor) {
    
    float intensity = computeIntensity(lightPos, intersectionPoint, normal, lightBrightness);
    
    Vec3 finalColor = applyLighting(sphereColor, intensity);
    
    printf("Final Color: R=%f, G=%f, B=%f\n", finalColor.x, finalColor.y, finalColor.z);
}

// function to read colors from a list of color strings and convert to vec3
void readColors(int numColors, char **colorList, Vec3 *colors) {
    for (int i = 0; i < numColors; i++) {
        unsigned int hexColor;
        //  convert from hex to int
        sscanf(colorList[i], "%x", &hexColor);
        colors[i].x = (hexColor >> 16) & 0xFF;
        colors[i].y = (hexColor >> 8) & 0xFF;
        colors[i].z = hexColor & 0xFF;
   
        printf("Color %d: R=%f, G=%f, B=%f\n", i, colors[i].x, colors[i].y, colors[i].z);
    }
}
void writeColour(FILE *ppmFile, Vec3 color) {
    int r = (int)(fmax(0.0f, fmin(1.0f, color.x)) * 255.0f); 
    int g = (int)(fmax(0.0f, fmin(1.0f, color.y)) * 255.0f); 
    int b = (int)(fmax(0.0f, fmin(1.0f, color.z)) * 255.0f);
    fprintf(ppmFile, "%d %d %d\n", r, g, b);  
}
