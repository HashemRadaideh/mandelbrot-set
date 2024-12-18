#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ZOOM_IN_FACTOR 0.9
#define ZOOM_OUT_FACTOR 1.1
#define MIN_LOD 50
#define LOD 100
#define MAX_LOD 5000

uint16_t width = 0, height = 0;

Vector2 dragStart = {0, 0};
bool isDragging = false;

double zoomLevel = 1;
uint64_t lod = LOD;

double realMin = -2.0, realMax = 2.0, imagMin = -1.5, imagMax = 1.5;
double prevRealMin = 0, prevRealMax = 0, prevImagMin = 0, prevImagMax = 0;

uint64_t prevLod = 0;
Color* colorPalette = NULL;

Color* pixels = NULL;
Texture2D texture;

bool isMenuOpen = false;

Rectangle resetButton = {100, 200, 150, 50};
Rectangle closeButton = {100, 300, 150, 50};

void initialize(uint16_t* width, uint16_t* height) {
  *width = GetScreenWidth();
  *height = GetScreenHeight();

  const double aspectRatio = (double)*height / (double)*width;
  const double realRange = realMax - realMin;
  const double imagRange = realRange * aspectRatio;
  const double imagCenter = (imagMin + imagMax) / 2.0;

  imagMin = imagCenter - imagRange / 2.0;
  imagMax = imagCenter + imagRange / 2.0;

  if (colorPalette) free(colorPalette);
  colorPalette = malloc(MAX_LOD * sizeof(Color));

  if (pixels) free(pixels);
  pixels = malloc((*width) * (*height) * sizeof(Color));

  if (IsTextureValid(texture)) UnloadTexture(texture);
  texture = LoadTextureFromImage(GenImageColor(*width, *height, BLANK));
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

  lod = fmax(MIN_LOD, fmin(MAX_LOD, LOD + (log10(zoomLevel) * MIN_LOD)));
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

void precomputeColor() {
  if (prevLod == lod) return;

  if (prevLod > lod) prevLod = 0;

  for (; prevLod <= lod; prevLod++) {
    colorPalette[prevLod] = mapColor(prevLod);
  }
}

uint64_t mandelbrot(const double real, const double imag) {
  double zReal = real, zImag = imag;
  double zReal2 = zReal * zReal, zImag2 = zImag * zImag;

  uint64_t iter = 0;
  for (iter = 0; zReal2 + zImag2 <= lod && iter < lod; iter++) {
    zImag = 2 * zReal * zImag + imag;
    zReal = zReal2 - zImag2 + real;

    zReal2 = zReal * zReal;
    zImag2 = zImag * zImag;
  }

  return iter;
}

void mandelbrotSet() {
  const double imagStep = (imagMax - imagMin) / (height - 1);
  const double realStep = (realMax - realMin) / (width - 1);

  for (uint16_t y = 0; y < height; ++y) {
    const double imag = imagMax - y * imagStep;

    for (uint16_t x = 0; x < width; ++x) {
      const double real = realMin + x * realStep;

      pixels[y * width + x] = colorPalette[mandelbrot(real, imag)];
    }
  }
}

void generateTexture() {
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

void telemetry() {
  const int barWidth = 20;
  const int barX = 10;
  const int barHeight = height - 20;
  const int barY = 10;

  for (int i = 0; i < barHeight; i++) {
    uint64_t paletteIndex = (i * lod) / barHeight;
    if (paletteIndex >= lod) paletteIndex = lod - 1;

    DrawRectangle(barX, barY + i, barWidth, 1, colorPalette[paletteIndex]);
  }

  DrawRectangleLines(barX, barY, barWidth, barHeight, WHITE);

  const int textX = barX + barWidth + 10;
  int textY = barY;

  char fpsText[20];
  sprintf(fpsText, "FPS: %d", GetFPS());
  DrawText(fpsText, textX, textY, 20, GREEN);
  textY += 30;

  char renderTimeText[30];
  sprintf(renderTimeText, "Render Time: %.2f ms", GetFrameTime() * 1000);
  DrawText(renderTimeText, textX, textY, 20, RED);
  textY += 30;

  char windowSizeText[40];
  sprintf(windowSizeText, "Resolution: %dx%d %f", width, height,
          (float)width / height);
  DrawText(windowSizeText, textX, textY, 20, GRAY);
  textY += 30;

  float dpiScale = GetWindowScaleDPI().x;
  char dpiScaleText[30];
  sprintf(dpiScaleText, "DPI Scale: %.2f", dpiScale);
  DrawText(dpiScaleText, textX, textY, 20, GREEN);
  textY += 30;

  char lodText[20];
  sprintf(lodText, "LOD: %lu", lod);
  DrawText(lodText, textX, textY, 20, YELLOW);
  textY += 30;

  char zoomText[30];
  sprintf(zoomText, "Zoom Level: %.2f", zoomLevel);
  DrawText(zoomText, textX, textY, 20, LIME);
  textY += 30;

  char coordText[80];
  sprintf(coordText, "Real: [%f, %f]\nImag: [%f, %f]", realMin, realMax,
          imagMin, imagMax);
  DrawText(coordText, textX, textY, 20, LIGHTGRAY);
  textY += 50;

  char dragOffsetText[50];
  double realOffset = realMax - realMin;
  double imagOffset = imagMax - imagMin;
  sprintf(dragOffsetText, "Drag Offset: [%.6f, %.6f]", realOffset, imagOffset);
  DrawText(dragOffsetText, textX, textY, 20, LIGHTGRAY);
  textY += 30;
}

void ResetToDefault() {
  realMin = -2.0, realMax = 2.0, imagMin = -1.5, imagMax = 1.5;
  zoomLevel = 1;
  lod = LOD;
  initialize(&width, &height);
}

void handleMenu() {
  Vector2 mousePosition = GetMousePosition();

  if (CheckCollisionPointRec(mousePosition, closeButton) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    CloseWindow();
  } else if (CheckCollisionPointRec(mousePosition, resetButton) &&
             IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    ResetToDefault();
    isMenuOpen = false;
  }
}

Texture2D blurTexture;

void openMenu() {
  const int menuWidth = 400;
  const int menuHeight = 300;

  const int menuX = (GetScreenWidth() - menuWidth) / 2;
  const int menuY = (GetScreenHeight() - menuHeight) / 2;

  DrawTexture(blurTexture, 0, 0, Fade(BLACK, 0.5f));

  DrawRectangle(menuX, menuY, menuWidth, menuHeight, Fade(BLACK, 0.9f));

  DrawRectangleLines(menuX, menuY, menuWidth, menuHeight, RAYWHITE);

  const int titleX = menuX + (menuWidth / 2) - MeasureText("Menu", 40) / 2;
  const int titleY = menuY + 20;
  DrawText("Menu", titleX, titleY, 40, RAYWHITE);

  resetButton.x = menuX + (menuWidth / 2.0) - (resetButton.width / 2);
  resetButton.y = menuY + 100;

  closeButton.x = menuX + (menuWidth / 2.0) - (closeButton.width / 2);
  closeButton.y = menuY + 200;

  DrawRectangleRec(resetButton, DARKGRAY);
  DrawText("Reset", resetButton.x + 20, resetButton.y + 10, 20, WHITE);

  DrawRectangleRec(closeButton, DARKGRAY);
  DrawText("Close", closeButton.x + 20, closeButton.y + 10, 20, WHITE);
}

void generateBlurOverlay(uint16_t width, uint16_t height) {
  Image blurImage = GenImagePerlinNoise(width / 10, height / 10, 0, 0, 5.0f);
  ImageResize(&blurImage, width, height);
  blurTexture = LoadTextureFromImage(blurImage);
  UnloadImage(blurImage);
}

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(width, height, "Mandelbrot Set");
  SetTargetFPS(60);
  SetExitKey(KEY_NULL);
  initialize(&width, &height);

  generateBlurOverlay(GetScreenWidth(), GetScreenHeight());

  while (!WindowShouldClose()) {
    if (IsWindowResized()) {
      initialize(&width, &height);
      generateBlurOverlay(GetScreenWidth(), GetScreenHeight());
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
      isMenuOpen = !isMenuOpen;
    }

    BeginDrawing();
    ClearBackground(BLACK);

    precomputeColor();
    generateTexture();
    DrawTexture(texture, 0, 0, WHITE);

    if (isMenuOpen) {
      handleMenu();
      openMenu();
    } else {
      handleZoom();
      handleDrag();
      telemetry();
    }

    EndDrawing();
  }

  UnloadTexture(blurTexture);
  UnloadTexture(texture);
  free(pixels);
  free(colorPalette);
  CloseWindow();
  return EXIT_SUCCESS;
}
