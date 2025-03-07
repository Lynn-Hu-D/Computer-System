/*
learn.c
Lingyu Hu
CS5600
*/
#include <stdio.h>
#include "mpg2km.h"


int main() {
    double mpg = 25.0; // Example input for miles per gallon
    double lph = 10.0; // Example input for liters per 100 kilometers

    // Output results
    printf("Test Case 1 - Valid Inputs:\n");
    printf("%.2f MPG is %.2f km/L\n", mpg, mpg2kml(mpg));
    printf("%.2f MPG is %.2f liters per 100 km\n", mpg, mpg2lphm(mpg));
    printf("%.2f liters per 100 km is %.2f MPG\n", lph, lph2mpg(lph));

    // Test case 2: Invalid mpg (zero)
    printf("\nTest Case 2 - Invalid MPG (Zero):\n");
    printf("Zero MPG results in %.2f km/L\n", mpg2kml(0.0));
    printf("Zero MPG results in %.2f liters per 100 km\n", mpg2lphm(0.0));

    // Test case 3: Invalid lph (zero)
    printf("\nTest Case 3 - Invalid LPH (Zero):\n");
    printf("Zero LPH results in %.2f MPG\n", lph2mpg(0.0));


    return 0;
}