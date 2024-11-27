#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>

#include "complex.h"

#define mapColor(p) \
  p == LOD ? BLACK : (Color){p * 5 % 256, p * 3 % 256, 255 - p % 256, 255}

int mandelbrot(Complex c, int LOD) {
  Complex z = {0, 0};
  int iter = 0;

  for (iter = 0; compMagnitude(z) <= 2 && iter < LOD; iter++) {
    z = compAdd(compMul(z, z), c);
  }

  return iter;
}

void mandelbrotSet(double realMin, double realMax, double imagMin,
                   double imagMax, size_t width, size_t height, int LOD,
                   Color* buffer) {
  const double realStep = (realMax - realMin) / (width - 1);
  const double imagStep = (imagMax - imagMin) / (height - 1);

  for (size_t index = 0; index < height * width; ++index) {
    double real = realMin + (index % width) * realStep;
    double imag = imagMax - ((double)index / width) * imagStep;

    buffer[index] = mapColor(mandelbrot((Complex){real, imag}, LOD));
  }
}
