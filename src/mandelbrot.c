#include <raylib.h>
#include <stdint.h>

#define map(iter) \
  iter == LOD     \
      ? BLACK     \
      : (Color){(iter * 5) & 255, (iter * 3) & 255, 255 - (iter & 255), 255}

inline uint64_t mandelbrot(double real, double imag, const uint64_t LOD) {
  double zReal = real;
  double zImag = imag;

  double zReal2 = zReal * zReal;
  double zImag2 = zImag * zImag;

  uint64_t iter = 0;
  for (iter = 0; zReal2 + zImag2 <= LOD && iter < LOD; iter++) {
    zImag = 2 * zReal * zImag + imag;
    zReal = zReal2 - zImag2 + real;

    zReal2 = zReal * zReal;
    zImag2 = zImag * zImag;
  }

  return iter;
}

void mandelbrotSet(const double realMin, const double realMax,
                   const double imagMin, const double imagMax,
                   const uint16_t width, const uint16_t height,
                   const uint64_t LOD, Color* pixles) {
  const double imagStep = (imagMax - imagMin) / (height - 1);
  double imagValues[height];
  for (uint16_t y = 0; y < height; ++y) imagValues[y] = imagMax - y * imagStep;

  const double realStep = (realMax - realMin) / (width - 1);
  double realValues[width];
  for (uint16_t x = 0; x < width; ++x) realValues[x] = realMin + x * realStep;

  for (uint16_t y = 0; y < height; ++y) {
    const double imag = imagValues[y];

    for (uint16_t x = 0; x < width; ++x) {
      const double real = realValues[x];

      uint64_t index = y * width + x;
      uint64_t iter = mandelbrot(real, imag, LOD);

      pixles[index] = map(iter);
    }
  }
}
