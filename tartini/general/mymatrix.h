#ifndef MYMATRIX_H
#define MYMATRIX_H

void print2x2matrix(const double *x);
void print3x3matrix(const double *x);
bool invert2x2matrix(const double *input, double *output);
bool invert3x3matrix(const double *x, double *y);
bool pinv(const float *x1, const float *y, int n, double *alpha);
bool pinv(const float *x1, const float *x2, const float *y, int n, double *alpha);
bool pinv(const float *x1, const float *x2, const float *x3, const float *y, int n, double *alpha);

/** Finds the determinant of a 2x2 matrix
  * [a b]
  * [c d]
  */
inline double determinant2x2(double a, double b, double c, double d)
{
  return a*d - b*c;
}

/** Finds the determinant of a 3x3 matrix
  * [0 1 2]
  * [3 4 5]
  * [6 7 8]
  */
inline double determinant3x3(const double *x) {
 return x[0]*x[4]*x[8] - x[0]*x[5]*x[7] - x[1]*x[3]*x[8] + x[1]*x[5]*x[6] + x[2]*x[3]*x[7] - x[2]*x[4]*x[6];
}

#endif
