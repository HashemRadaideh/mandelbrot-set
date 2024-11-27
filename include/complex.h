#pragma once

typedef struct {
  double Real;
  double Imag;
} Complex;

Complex compMul(Complex, Complex);
Complex compAdd(Complex, Complex);
double compMagnitude(Complex);
