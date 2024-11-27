#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "mandelbrot.h"

static const char* title = "Mandelbrot Set";
static const int screenWidth = 800;
static const int screenHeight = 800;

static const size_t maxIter = 50;
double realMin = -2.0, realMax = 1.0, imagMin = -1.5, imagMax = 1.5;

double prevRealMin = 0, prevRealMax = 0, prevImagMin = 0, prevImagMax = 0;

Texture2D mandelbrotTexture;
Image mandelbrotImage;

void setup() {
  InitWindow(screenWidth, screenHeight, title);
  SetTargetFPS(60);

  mandelbrotImage = GenImageColor(screenWidth, screenHeight, BLACK);
  mandelbrotTexture = LoadTextureFromImage(mandelbrotImage);
}

void handleZoom() {
  const float scroll = GetMouseWheelMove();

  if (scroll != 0) {
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
}

void handleDrag() {
  static Vector2 dragStart = {0, 0};
  static bool isDragging = false;

  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    const Vector2 currentMousePos = GetMousePosition();

    if (!isDragging) {
      dragStart = currentMousePos;
      isDragging = true;
    } else {
      const Vector2 dragDelta = Vector2Subtract(dragStart, currentMousePos);

      const double realRange = realMax - realMin;
      const double imagRange = imagMax - imagMin;

      realMin += (dragDelta.x / screenWidth) * realRange;
      realMax += (dragDelta.x / screenWidth) * realRange;
      imagMin -= (dragDelta.y / screenHeight) * imagRange;
      imagMax -= (dragDelta.y / screenHeight) * imagRange;

      dragStart = currentMousePos;
    }
  } else {
    isDragging = false;
  }
}

void handleInput() {
  handleZoom();

  handleDrag();
}

void updateMandelbrotSet() {
  if (prevRealMin == realMin && prevRealMax == realMax &&
      prevImagMin == imagMin && prevImagMax == imagMax) {
    return;
  }

  prevRealMin = realMin;
  prevRealMax = realMax;
  prevImagMin = imagMin;
  prevImagMax = imagMax;

  mandelbrotSet(realMin, realMax, imagMin, imagMax, screenWidth, screenHeight,
                maxIter, mandelbrotImage.data);

  UpdateTexture(mandelbrotTexture, mandelbrotImage.data);
}

void update() {
  updateMandelbrotSet();

  DrawTexture(mandelbrotTexture, 0, 0, WHITE);
}

void clean() {
  UnloadTexture(mandelbrotTexture);
  UnloadImage(mandelbrotImage);

  CloseWindow();
}

int main(int argc, char* argv[]) {
  setup();
  while (!WindowShouldClose()) {
    handleInput();
    BeginDrawing();
    ClearBackground(BLACK);
    update();
    EndDrawing();
  }
  clean();
  return EXIT_SUCCESS;
}
