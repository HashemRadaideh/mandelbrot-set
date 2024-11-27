#pragma once

#include <stdlib.h>

#include "complex.h"

Complex mandelbrot(Complex, size_t);

void mandelbrotSet(double, double, double, double, size_t, size_t, size_t,
                   int *);
