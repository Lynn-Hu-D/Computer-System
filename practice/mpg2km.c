#include "mpg2km.h"

double mpg2kml(double mpg) {
    if (mpg <= 0) {
        return -1;
    }
    return mpg * 0.425144; // Conversion factor

}

double mpg2lphm(double mpg) {
  if (mpg == 0) {
        return -1;
    }
    return 235.214583 / mpg; // Conversion factor
}

double lph2mpg(double lph) {
    if (lph == 0) {
        return -1;
    }
    return 235.214583 / lph; // Conversion factor
}