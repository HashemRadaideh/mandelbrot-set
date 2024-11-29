#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define ZOOM_IN_FACTOR 0.9
#define ZOOM_OUT_FACTOR 1.1
#define LOD 50
#define MIN_LOD 10
#define MAX_LOD 2000

uint16_t width = 0, height = 0;

double realMin = -2.0, realMax = 2.0, imagMin = -1.5, imagMax = 1.5;
double prevRealMin = 0, prevRealMax = 0, prevImagMin = 0, prevImagMax = 0;

double zoomLevel = 1;
uint64_t lod = LOD;

Color* pixels = NULL;
Texture2D texture;

void initialize(uint16_t* width, uint16_t* height) {
  *width = GetScreenWidth();
  *height = GetScreenHeight();

  const double aspectRatio = (double)*height / (double)*width;
  const double realRange = realMax - realMin;
  const double imagCenter = (imagMin + imagMax) / 2.0;
  const double imagRange = realRange * aspectRatio;

  imagMin = imagCenter - imagRange / 2.0;
  imagMax = imagCenter + imagRange / 2.0;

  if (pixels) free(pixels);
  pixels = malloc((*width) * (*height) * sizeof(Color));

  if (IsTextureValid(texture)) UnloadTexture(texture);
  texture = LoadTextureFromImage(GenImageColor(*width, *height, BLANK));
}

void handleZoom() {
  float scroll = GetMouseWheelMove();
  if (scroll == 0) return;

  const double zoomFactor = (scroll > 0) ? ZOOM_IN_FACTOR : ZOOM_OUT_FACTOR;
  zoomLevel = zoomLevel / zoomFactor > 0 ? zoomLevel / zoomFactor : zoomLevel;

  const double realCenter = (realMin + realMax) / 2.0;
  const double imagCenter = (imagMin + imagMax) / 2.0;

  const double realRange = (realMax - realMin) * zoomFactor / 2.0;
  const double imagRange = (imagMax - imagMin) * zoomFactor / 2.0;

  realMin = realCenter - realRange;
  realMax = realCenter + realRange;
  imagMin = imagCenter - imagRange;
  imagMax = imagCenter + imagRange;

  lod = LOD + (log10(zoomLevel) * (width + height) / 100);
  lod = fmax(MIN_LOD, fmin(MAX_LOD, lod));
}

void handleDrag() {
  static Vector2 dragStart = {0, 0};
  static bool isDragging = false;

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    dragStart = GetMousePosition();
    isDragging = true;
  } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    isDragging = false;
  }

  if (!isDragging) return;

  const Vector2 dragDelta = Vector2Subtract(dragStart, GetMousePosition());
  dragStart = GetMousePosition();

  const double realRange = realMax - realMin;
  const double imagRange = imagMax - imagMin;

  realMin += (dragDelta.x / width) * realRange;
  realMax += (dragDelta.x / width) * realRange;
  imagMin -= (dragDelta.y / height) * imagRange;
  imagMax -= (dragDelta.y / height) * imagRange;
}

inline uint64_t mandelbrot(double real, double imag) {
  double zReal = real;
  double zImag = imag;

  double zReal2 = zReal * zReal;
  double zImag2 = zImag * zImag;

  uint64_t iter = 0;
  for (iter = 0; zReal2 + zImag2 <= lod && iter < lod; iter++) {
    zImag = 2 * zReal * zImag + imag;
    zReal = zReal2 - zImag2 + real;

    zReal2 = zReal * zReal;
    zImag2 = zImag * zImag;
  }

  return iter;
}

Color mapColor(uint64_t iter) {
  return (iter == lod) ? BLACK
                       : (Color){(iter * 5) & 255, (iter * 3) & 255,
                                 255 - (iter & 255), 255};
}

void mandelbrotSet() {
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
      uint64_t iter = mandelbrot(real, imag);

      pixels[index] = mapColor(iter);
    }
  }
}

void updateSetTexture() {
  if (prevRealMin == realMin && prevRealMax == realMax &&
      prevImagMin == imagMin && prevImagMax == imagMax) {
    return;
  }

  prevRealMin = realMin;
  prevRealMax = realMax;
  prevImagMin = imagMin;
  prevImagMax = imagMax;

  mandelbrotSet();
  UpdateTexture(texture, pixels);
}

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(width, height, "Mandelbrot Set");
  SetTargetFPS(60);

  initialize(&width, &height);
  while (!WindowShouldClose()) {
    if (IsWindowResized()) {
      initialize(&width, &height);
    }

    handleZoom();
    handleDrag();

    BeginDrawing();
    ClearBackground(BLACK);

    updateSetTexture();
    DrawTexture(texture, 0, 0, WHITE);
    EndDrawing();
  }

  UnloadTexture(texture);
  free(pixels);
  CloseWindow();
  return EXIT_SUCCESS;
}
