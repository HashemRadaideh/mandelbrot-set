#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "mandelbrot.h"

double realMin = -2.0, realMax = 2.0, imagMin = -1.5, imagMax = 1.5;
double prevRealMin = 0, prevRealMax = 0, prevImagMin = 0, prevImagMax = 0;

Texture2D mandelbrotTexture;
Image mandelbrotImage;

void setup(int* screenWidth, int* screenHeight) {
  *screenWidth = GetScreenWidth();
  *screenHeight = GetScreenHeight();

  const double aspectRatio = (double)*screenHeight / (double)*screenWidth;

  const double realRange = realMax - realMin;
  const double imagCenter = (imagMin + imagMax) / 2.0;
  const double imagRange = realRange * aspectRatio;

  imagMin = imagCenter - imagRange / 2.0;
  imagMax = imagCenter + imagRange / 2.0;

  mandelbrotImage = GenImageColor(*screenWidth, *screenHeight, BLANK);
  mandelbrotTexture = LoadTextureFromImage(mandelbrotImage);
}

void handleZoom() {
  const float scroll = GetMouseWheelMove();

  if (scroll == 0) {
    return;
  }

  const double realCenter = (realMin + realMax) / 2.0;
  const double imagCenter = (imagMin + imagMax) / 2.0;

  const double rangeFactor = (scroll > 0) ? 0.9 : 1.1;

  const double realRange = (realMax - realMin) * rangeFactor / 2.0;
  const double imagRange = (imagMax - imagMin) * rangeFactor / 2.0;

  realMin = realCenter - realRange;
  realMax = realCenter + realRange;
  imagMin = imagCenter - imagRange;
  imagMax = imagCenter + imagRange;
}

void handleDrag(const int screenWidth, const int screenHeight) {
  static Vector2 dragStart = {0, 0};
  static bool isDragging = false;

  if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    isDragging = false;
    return;
  }

  const Vector2 currentMousePos = GetMousePosition();

  if (!isDragging) {
    dragStart = currentMousePos;
    isDragging = true;
    return;
  }

  const Vector2 dragDelta = Vector2Subtract(dragStart, currentMousePos);
  dragStart = currentMousePos;

  const double realRange = realMax - realMin;
  const double imagRange = imagMax - imagMin;

  realMin += (dragDelta.x / screenWidth) * realRange;
  realMax += (dragDelta.x / screenWidth) * realRange;
  imagMin -= (dragDelta.y / screenHeight) * imagRange;
  imagMax -= (dragDelta.y / screenHeight) * imagRange;
}

void handleInput(const int screenWidth, const int screenHeight) {
  handleZoom();

  handleDrag(screenWidth, screenHeight);
}

void updateMandelbrotSet(const int screenWidth, const int screenHeight) {
  if (prevRealMin == realMin && prevRealMax == realMax &&
      prevImagMin == imagMin && prevImagMax == imagMax) {
    return;
  }

  prevRealMin = realMin;
  prevRealMax = realMax;
  prevImagMin = imagMin;
  prevImagMax = imagMax;

  static const size_t maxIter = 50;

  mandelbrotSet(realMin, realMax, imagMin, imagMax, screenWidth, screenHeight,
                maxIter, mandelbrotImage.data);

  UpdateTexture(mandelbrotTexture, mandelbrotImage.data);
}

void update(const int screenWidth, const int screenHeight) {
  updateMandelbrotSet(screenWidth, screenHeight);
  DrawTexture(mandelbrotTexture, 0, 0, WHITE);
}

void clean() {
  UnloadTexture(mandelbrotTexture);
  UnloadImage(mandelbrotImage);
}

int main() {
  static const char* title = "Mandelbrot Set";
  static int screenWidth = 800;
  static int screenHeight = 600;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(screenWidth, screenHeight, title);
  SetTargetFPS(60);

  setup(&screenWidth, &screenHeight);
  while (!WindowShouldClose()) {
    if (IsWindowResized()) {
      clean();
      setup(&screenWidth, &screenHeight);
    }

    handleInput(screenWidth, screenHeight);
    BeginDrawing();
    update(screenWidth, screenHeight);
    EndDrawing();
  }
  clean();

  CloseWindow();
  return EXIT_SUCCESS;
}
