#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>

#include "complex.h"

#define pixelColor(p) \
  p == maxIter ? BLACK : (Color){p * 5 % 256, p * 3 % 256, 255 - p % 256, 255}

Color mandelbrot(Complex c, size_t maxIter) {
  Complex z = {0, 0};
  size_t iter = 0;

  for (iter = 0; compMagnitude(z) <= 2 && iter < maxIter; iter++) {
    z = compAdd(compMul(z, z), c);
  }

  return pixelColor(iter);
}

void mandelbrotSet(double realMin, double realMax, double imagMin,
                   double imagMax, size_t width, size_t height, size_t maxIter,
                   Color* grid) {
  const double realStep = (realMax - realMin) / (width - 1);
  const double imagStep = (imagMax - imagMin) / (height - 1);

  double imag = imagMax - 0 * imagStep;
  double real = realMin + 0 * realStep;

  for (size_t i = 0; i < height; ++i) {
    for (size_t j = 0; j < width; ++j) {
      grid[i * width + j] = mandelbrot((Complex){real, imag}, maxIter);

      real = realMin + j * realStep;
    }

    imag = imagMax - i * imagStep;
  }
}
