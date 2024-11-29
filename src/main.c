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

float realMin = -2.0, realMax = 2.0, imagMin = -1.5, imagMax = 1.5;
float prevRealMin = 0, prevRealMax = 0, prevImagMin = 0, prevImagMax = 0;

float zoomLevel = 1;
uint64_t lod = LOD;

Color* pixels = NULL;
Texture2D texture;

void initialize(uint16_t* width, uint16_t* height) {
  *width = GetScreenWidth();
  *height = GetScreenHeight();

  const float aspectRatio = (float)*height / (float)*width;
  const float realRange = realMax - realMin;
  const float imagCenter = (imagMin + imagMax) / 2.0;
  const float imagRange = realRange * aspectRatio;

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

  const float zoomFactor = (scroll > 0) ? ZOOM_IN_FACTOR : ZOOM_OUT_FACTOR;
  zoomLevel = zoomLevel / zoomFactor > 0 ? zoomLevel / zoomFactor : zoomLevel;

  const float realCenter = (realMin + realMax) / 2.0;
  const float imagCenter = (imagMin + imagMax) / 2.0;

  const float realRange = (realMax - realMin) * zoomFactor / 2.0;
  const float imagRange = (imagMax - imagMin) * zoomFactor / 2.0;

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

  const Vector2 dragDelta = Vector2Subtract(GetMousePosition(), dragStart);
  dragStart = GetMousePosition();

  const float realRange = realMax - realMin;
  const float imagRange = imagMax - imagMin;

  const float realOffset = (dragDelta.x / width) * realRange;
  const float imagOffset = (dragDelta.y / height) * imagRange;

  realMin -= realOffset;
  realMax -= realOffset;
  imagMin += imagOffset;
  imagMax += imagOffset;
}

inline uint64_t mandelbrot(float real, float imag) {
  float zReal = real;
  float zImag = imag;

  float zReal2 = zReal * zReal;
  float zImag2 = zImag * zImag;

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
  return (Color){(iter * 5) & 255, (iter * 3) & 255, (255 - iter) & 255, 255};
}

void mandelbrotSet() {
  const float imagStep = (imagMax - imagMin) / (height - 1);
  float imagValues[height];
  for (uint16_t y = 0; y < height; ++y) imagValues[y] = imagMax - y * imagStep;

  const float realStep = (realMax - realMin) / (width - 1);
  float realValues[width];
  for (uint16_t x = 0; x < width; ++x) realValues[x] = realMin + x * realStep;

  for (uint16_t y = 0; y < height; ++y) {
    const float imag = imagValues[y];

    for (uint16_t x = 0; x < width; ++x) {
      const float real = realValues[x];

      uint64_t index = y * width + x;
      uint64_t iter = mandelbrot(real, imag);
      Color color = iter == lod ? BLACK : mapColor(iter);

      pixels[index] = color;
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
