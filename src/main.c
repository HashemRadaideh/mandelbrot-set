#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "mandelbrot.h"

char* title = "Mandelbrot Set";
const int screenWidth = 800;
const int screenHeight = 800;

double zoomFactor = 1.0;
Vector2 dragStart = {0, 0};
bool isDragging = false;

double realMin = -2.0, realMax = 1.0, imagMin = -1.5, imagMax = 1.5;
const size_t maxIter = 50;
int* grid;

double prevRealMin = 0, prevRealMax = 0, prevImagMin = 0, prevImagMax = 0;

void setup() {
  InitWindow(screenWidth, screenHeight, title);
  SetTargetFPS(60);
  ClearBackground(BLACK);

  grid = malloc(GetScreenHeight() * GetScreenWidth() * sizeof(int*));
}

void handleInput() {
  float scroll = GetMouseWheelMove();
  if (scroll != 0) {
    double realCenter = (realMin + realMax) / 2.0;
    double imagCenter = (imagMin + imagMax) / 2.0;
    double realRange = (realMax - realMin) * 0.5;
    double imagRange = (imagMax - imagMin) * 0.5;

    if (scroll > 0) {
      realRange *= 0.9;
      imagRange *= 0.9;
    } else {
      realRange *= 1.1;
      imagRange *= 1.1;
    }

    realMin = realCenter - realRange;
    realMax = realCenter + realRange;
    imagMin = imagCenter - imagRange;
    imagMax = imagCenter + imagRange;
  }

  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    Vector2 currentMousePos = GetMousePosition();
    if (!isDragging) {
      dragStart = currentMousePos;
      isDragging = true;
    } else {
      Vector2 dragDelta = Vector2Subtract(dragStart, currentMousePos);

      double realRange = realMax - realMin;
      double imagRange = imagMax - imagMin;

      realMin += (dragDelta.x / GetScreenWidth()) * realRange;
      realMax += (dragDelta.x / GetScreenWidth()) * realRange;
      imagMin -= (dragDelta.y / GetScreenHeight()) * imagRange;
      imagMax -= (dragDelta.y / GetScreenHeight()) * imagRange;

      dragStart = currentMousePos;
    }
  } else {
    isDragging = false;
  }
}

void updateMandelbrotSet() {
  if (prevRealMin == realMin && prevRealMax == realMax &&
      prevImagMin == imagMin && prevImagMax == imagMax) {
    return;
  }

  mandelbrotSet(realMin, realMax, imagMin, imagMax, GetScreenWidth(),
                GetScreenHeight(), maxIter, grid);

  prevRealMin = realMin;
  prevRealMax = realMax;
  prevImagMin = imagMin;
  prevImagMax = imagMax;
}

void update() {
  updateMandelbrotSet();

  for (int i = 0; i < GetScreenHeight() * GetScreenWidth(); ++i) {
    size_t iter = grid[i];

    Color color = iter == maxIter ? BLACK
                                  : (Color){iter * 4 % 256, iter * 2 % 256,
                                            255 - iter % 256, 255};

    DrawPixel(i % GetScreenWidth(), i / GetScreenHeight(), color);
  }
}

void clean() {
  free(grid);
  CloseWindow();
}

int main(int argc, char* argv[]) {
  setup();
  while (!WindowShouldClose()) {
    handleInput();
    BeginDrawing();
    update();
    EndDrawing();
  }
  clean();
  return EXIT_SUCCESS;
}
