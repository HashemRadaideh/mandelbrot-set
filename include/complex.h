#pragma once

typedef struct {
  double real;
  double imag;
} Complex;

Complex compMul(const Complex, const Complex);
Complex compAdd(const Complex, const Complex);
