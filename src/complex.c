#include "complex.h"

#include <math.h>
#include <stdio.h>

Complex compMul(Complex left, Complex right) {
  return (Complex){.Real = left.Real * right.Real - left.Imag * right.Imag,
                   .Imag = left.Real * right.Imag + left.Imag * right.Real};
}

Complex compAdd(Complex left, Complex right) {
  return (Complex){.Real = left.Real + right.Real,
                   .Imag = left.Imag + right.Imag};
}

Complex compSub(Complex left, Complex right) {
  return (Complex){.Real = left.Real - right.Real,
                   .Imag = left.Imag - right.Imag};
}

Complex compDiv(Complex left, Complex right) {
  double denominator = right.Real * right.Real + right.Imag * right.Imag;
  if (denominator == 0) {
    fprintf(stderr, "Error: Division by zero in complex division.\n");
    return (Complex){0, 0};
  }
  return (Complex){
      .Real = (left.Real * right.Real + left.Imag * right.Imag) / denominator,
      .Imag = (left.Imag * right.Real - left.Real * right.Imag) / denominator};
}

double compMagnitude(Complex c) {
  return sqrt(c.Real * c.Real + c.Imag * c.Imag);
}

Complex compConjugate(Complex c) {
  return (Complex){.Real = c.Real, .Imag = -c.Imag};
}

Complex compNegate(Complex c) {
  return (Complex){.Real = -c.Real, .Imag = -c.Imag};
}

int compEqual(Complex left, Complex right) {
  return (left.Real == right.Real && left.Imag == right.Imag);
}

Complex compNormalize(Complex c) {
  double magnitude = compMagnitude(c);
  if (magnitude == 0) {
    fprintf(stderr, "Error: Cannot normalize a zero vector.\n");
    return (Complex){0, 0};
  }
  return (Complex){.Real = c.Real / magnitude, .Imag = c.Imag / magnitude};
}

void compPrint(Complex c) { printf("(%.2f + %.2fi)\n", c.Real, c.Imag); }
