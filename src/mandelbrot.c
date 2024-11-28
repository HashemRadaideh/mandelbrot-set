#include <raylib.h>
#include <stdint.h>

#include "complex.h"

#define compMagPow2(z) (z.real * z.real + z.imag * z.imag)

#define map(iter) \
  iter == LOD     \
      ? BLACK     \
      : (Color){(iter * 5) & 255, (iter * 3) & 255, 255 - (iter & 255), 255}

inline uint64_t mandelbrot(const Complex c, const uint64_t LOD) {
  uint64_t iter = 0;
  Complex z = c;

  for (; compMagPow2(z) <= LOD && iter < LOD; iter++) {
    z = compAdd(compMul(z, z), c);
  }

  return iter;
}

void mandelbrotSet(const double realMin, const double realMax,
                   const double imagMin, const double imagMax,
                   const uint16_t width, const uint16_t height,
                   const uint64_t LOD, Color* pixles) {
  const double realStep = (realMax - realMin) / (width - 1);
  const double imagStep = (imagMax - imagMin) / (height - 1);

  double imagValues[height];
  double realValues[width];
  for (uint16_t y = 0; y < height; ++y) imagValues[y] = imagMax - y * imagStep;
  for (uint16_t x = 0; x < width; ++x) realValues[x] = realMin + x * realStep;

  for (uint16_t y = 0; y < height; ++y) {
    const double imag = imagValues[y];
    for (uint16_t x = 0; x < width; ++x) {
      const double real = realValues[x];

      const Complex c = {real, imag};
      uint64_t iter = mandelbrot(c, LOD);
      uint64_t index = y * width + x;

      pixles[index] = map(iter);
    }
  }
}
