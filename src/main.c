#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define ZOOM_IN_FACTOR 0.9
#define ZOOM_OUT_FACTOR 1.1
#define LOD 50
#define MIN_LOD 10
#define MAX_LOD 5000

uint16_t width = 0, height = 0;

double realMin = -2.0, realMax = 2.0, imagMin = -1.5, imagMax = 1.5;
double prevRealMin = 0, prevRealMax = 0, prevImagMin = 0, prevImagMax = 0;

double zoomLevel = 1;
uint64_t lod = LOD;

Color* pixels = NULL;
Texture2D texture;

Vector2 dragStart = {0, 0};
bool isDragging = false;

void initialize(uint16_t* width, uint16_t* height) {
  *width = GetScreenWidth();
  *height = GetScreenHeight();

  const double aspectRatio = (double)*height / (double)*width;
  const double realRange = realMax - realMin;
  const double imagRange = realRange * aspectRatio;
  const double imagCenter = (imagMin + imagMax) / 2.0;

  imagMin = imagCenter - imagRange / 2.0;
  imagMax = imagCenter + imagRange / 2.0;

  if (pixels) free(pixels);
  pixels = malloc((*width) * (*height) * sizeof(Color));

  if (IsTextureValid(texture)) UnloadTexture(texture);
  texture = LoadTextureFromImage(GenImageColor(*width, *height, BLANK));
}

uint64_t mandelbrot(const double real, const double imag) {
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

Color mapColor(const int iter) {
  const double l = 400.0 + 300.0 * iter / lod;
  double r = 0.0, g = 0.0, b = 0.0, t = 0.0;

  if ((l >= 400.0) && (l < 410.0)) {
    t = (l - 400.0) / (410.0 - 400.0);
    r = +(0.33 * t) - (0.20 * t * t);
  } else if ((l >= 410.0) && (l < 475.0)) {
    t = (l - 410.0) / (475.0 - 410.0);
    r = 0.14 - (0.13 * t * t);
  } else if ((l >= 545.0) && (l < 595.0)) {
    t = (l - 545.0) / (595.0 - 545.0);
    r = +(1.98 * t) - (t * t);
  } else if ((l >= 595.0) && (l < 650.0)) {
    t = (l - 595.0) / (650.0 - 595.0);
    r = 0.98 + (0.06 * t) - (0.40 * t * t);
  } else if ((l >= 650.0) && (l < 700.0)) {
    t = (l - 650.0) / (700.0 - 650.0);
    r = 0.65 - (0.84 * t) + (0.20 * t * t);
  }

  if ((l >= 415.0) && (l < 475.0)) {
    t = (l - 415.0) / (475.0 - 415.0);
    g = +(0.80 * t * t);
  } else if ((l >= 475.0) && (l < 590.0)) {
    t = (l - 475.0) / (590.0 - 475.0);
    g = 0.8 + (0.76 * t) - (0.80 * t * t);
  } else if ((l >= 585.0) && (l < 639.0)) {
    t = (l - 585.0) / (639.0 - 585.0);
    g = 0.84 - (0.84 * t);
  }

  if ((l >= 400.0) && (l < 475.0)) {
    t = (l - 400.0) / (475.0 - 400.0);
    b = +(2.20 * t) - (1.50 * t * t);
  } else if ((l >= 475.0) && (l < 560.0)) {
    t = (l - 475.0) / (560.0 - 475.0);
    b = 0.7 - (t) + (0.30 * t * t);
  }

  return (Color){r * 255, g * 255, b * 255, 255};
}

void mandelbrotSet() {
  const double imagStep = (imagMax - imagMin) / (height - 1);
  const double realStep = (realMax - realMin) / (width - 1);

  for (uint16_t y = 0; y < height; ++y) {
    const double imag = imagMax - y * imagStep;

    for (uint16_t x = 0; x < width; ++x) {
      const double real = realMin + x * realStep;

      uint64_t iter = mandelbrot(real, imag);

      pixels[y * width + x] = mapColor(iter);
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

void handleZoom() {
  const double scroll = GetMouseWheelMove();
  if (!scroll) return;

  const double zoomFactor = scroll > 0 ? ZOOM_IN_FACTOR : ZOOM_OUT_FACTOR;
  zoomLevel = zoomLevel / zoomFactor > 0 ? zoomLevel / zoomFactor : zoomLevel;

  const double realCenter = (realMin + realMax) / 2.0;
  const double imagCenter = (imagMin + imagMax) / 2.0;

  const double realRange = (realMax - realMin) * zoomFactor / 2.0;
  const double imagRange = (imagMax - imagMin) * zoomFactor / 2.0;

  realMin = realCenter - realRange;
  realMax = realCenter + realRange;
  imagMin = imagCenter - imagRange;
  imagMax = imagCenter + imagRange;

  lod = LOD + (log10(zoomLevel) * 20);
  lod = fmax(MIN_LOD, lod);
}

void handleDrag() {
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    dragStart = GetMousePosition();
    isDragging = true;
  } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    isDragging = false;
  }

  if (!isDragging) return;

  const Vector2 dragDelta = Vector2Subtract(GetMousePosition(), dragStart);
  dragStart = GetMousePosition();

  const double realRange = realMax - realMin;
  const double imagRange = imagMax - imagMin;

  const double realOffset = (dragDelta.x / width) * realRange;
  const double imagOffset = (dragDelta.y / height) * imagRange;

  const double realCenter = (realMin + realMax) / 2.0 - realOffset;
  const double imagCenter = (imagMin + imagMax) / 2.0 + imagOffset;

  realMin = realCenter - realRange / 2.0;
  realMax = realCenter + realRange / 2.0;
  imagMin = imagCenter - imagRange / 2.0;
  imagMax = imagCenter + imagRange / 2.0;
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

    BeginDrawing();
    ClearBackground(BLACK);
    updateSetTexture();
    DrawTexture(texture, 0, 0, WHITE);
    EndDrawing();

    handleZoom();
    handleDrag();
  }

  UnloadTexture(texture);
  free(pixels);
  CloseWindow();
  return EXIT_SUCCESS;
}
