#pragma once

typedef struct {
  double Real;
  double Imag;
} Complex;

Complex compMul(Complex, Complex);
Complex compAdd(Complex, Complex);
Complex compSub(Complex, Complex);
Complex compDiv(Complex, Complex);
Complex compConjugate(Complex);
Complex compNegate(Complex);
Complex compNormalize(Complex);
double compMagnitude(Complex);
int compEqual(Complex, Complex);
void compPrint(Complex);
