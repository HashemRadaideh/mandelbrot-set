#include "complex.h"

#include <math.h>

Complex compMul(Complex left, Complex right) {
  return (Complex){.Real = left.Real * right.Real - left.Imag * right.Imag,
                   .Imag = left.Real * right.Imag + left.Imag * right.Real};
}

Complex compAdd(Complex left, Complex right) {
  return (Complex){.Real = left.Real + right.Real,
                   .Imag = left.Imag + right.Imag};
}

double compMagnitude(Complex c) {
  return sqrt(c.Real * c.Real + c.Imag * c.Imag);
}
