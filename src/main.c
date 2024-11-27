#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdlib.h>

#include "mandelbrot.h"

#define ZOOM_IN_FACTOR 0.9
#define ZOOM_OUT_FACTOR 1.1
#define LOD 50
#define MIN_LOD 10
#define MAX_LOD 2000

double realMin = -2.0, realMax = 2.0, imagMin = -1.5, imagMax = 1.5;
double prevRealMin = 0, prevRealMax = 0, prevImagMin = 0, prevImagMax = 0;

double zoomLevel = 1;
int lod = LOD;

Color* buffer = NULL;
Texture2D texture;

void setup(int* width, int* height) {
  *width = GetScreenWidth();
  *height = GetScreenHeight();

  const double aspectRatio = (double)*height / (double)*width;
  const double realRange = realMax - realMin;
  const double imagCenter = (imagMin + imagMax) / 2.0;
  const double imagRange = realRange * aspectRatio;

  imagMin = imagCenter - imagRange / 2.0;
  imagMax = imagCenter + imagRange / 2.0;

  if (buffer) free(buffer);
  buffer = malloc((*width) * (*height) * sizeof(Color));

  if (IsTextureValid(texture)) UnloadTexture(texture);
  texture = LoadTextureFromImage(GenImageColor(*width, *height, BLANK));
}

void handleZoom(const int width, const int height) {
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

  lod = LOD + round(log10(zoomLevel) * (width + height) / 100);
  lod = fmax(MIN_LOD, fmin(MAX_LOD, lod));
}

void handleDrag(const int width, const int height) {
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

void handleInput(const int width, const int height) {
  handleZoom(width, height);
  handleDrag(width, height);
}

void updateMandelbrotSet(const int width, const int height) {
  if (prevRealMin == realMin && prevRealMax == realMax &&
      prevImagMin == imagMin && prevImagMax == imagMax) {
    return;
  }

  prevRealMin = realMin;
  prevRealMax = realMax;
  prevImagMin = imagMin;
  prevImagMax = imagMax;

  mandelbrotSet(realMin, realMax, imagMin, imagMax, width, height, lod, buffer);
  UpdateTexture(texture, buffer);
}

void update(const int width, const int height) {
  updateMandelbrotSet(width, height);
  DrawTexture(texture, 0, 0, WHITE);
}

void clean() {
  UnloadTexture(texture);
  free(buffer);
}

int main() {
  static const char* title = "Mandelbrot Set";
  static int width = 800, height = 600;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(width, height, title);
  SetTargetFPS(60);

  setup(&width, &height);
  while (!WindowShouldClose()) {
    if (IsWindowResized()) {
      setup(&width, &height);
    }

    handleInput(width, height);
    BeginDrawing();
    ClearBackground(BLACK);
    update(width, height);
    EndDrawing();
  }

  clean();
  CloseWindow();
  return EXIT_SUCCESS;
}
