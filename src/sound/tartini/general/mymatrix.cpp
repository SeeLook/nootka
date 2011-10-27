#include "mymatrix.h"
#include "stdio.h"
#include "cmath"
#include "algorithm"

#define MY_EPSILON 0.000001

void print2x2matrix(const double *x)
{
  printf("[%8.5lf %8.5lf]\n", x[0], x[1]);
  printf("[%8.5lf %8.5lf]\n", x[2], x[3]);
}

void print3x3matrix(const double *x)
{
  printf("[%8.5lf %8.5lf %8.5lf]\n", x[0], x[1], x[2]);
  printf("[%8.5lf %8.5lf %8.5lf]\n", x[3], x[4], x[5]);
  printf("[%8.5lf %8.5lf %8.5lf]\n", x[6], x[7], x[8]);
}

/** Matrix inverse assuming the array follows this index pattern.
  * [0  1]
  * [2  3]
  * @return true if the matrix was inverted. false if no solution.
  */
bool invert2x2matrix(const double *input, double *output)
{
  double det = determinant2x2(input[0], input[1], input[2], input[3]);
  if(fabs(det) < MY_EPSILON) return false;
  output[0] = input[3] / det;
  output[1] = -input[1] / det;
  output[2] = -input[2] / det;
  output[3] = input[0] / det;
  return true;
}

/** Matrix inverse assuming the array follows this index pattern.
  * [0 1 2]
  * [3 4 5]
  * [6 7 8]
  * @param x matrix to invert
  * @param y stores the result in y
  * @return true if the matrix was inverted. false if no solution.
  */
bool invert3x3matrix(const double *x, double *y)
{
  double det = determinant3x3(x);
  if(fabs(det) < MY_EPSILON) return false;
  y[0] = determinant2x2(x[4], x[5], x[7], x[8]) / det;
  y[1] = determinant2x2(x[2], x[1], x[8], x[7]) / det;
  y[2] = determinant2x2(x[1], x[2], x[4], x[5]) / det;
  y[3] = determinant2x2(x[5], x[3], x[8], x[6]) / det;
  y[4] = determinant2x2(x[0], x[2], x[6], x[8]) / det;
  y[5] = determinant2x2(x[2], x[0], x[5], x[3]) / det;
  y[6] = determinant2x2(x[3], x[4], x[6], x[7]) / det;
  y[7] = determinant2x2(x[1], x[0], x[7], x[6]) / det;
  y[8] = determinant2x2(x[0], x[1], x[3], x[4]) / det;
  return true;
}

/** Moore-Penrose Matrix-inverse (or pseudo-inverse) such that X*alpha1 - Y is minimised using least squares
  * Ref: http://mathworld.wolfram.com/Moore-PenroseMatrixInverse.html
  * @param x1 The first (and only) column of X. An array of length n to fit to y
  * @param y The first (and only) column of Y. Y is the matrix to be fitted to, using least squares
  * @param n the number of items in x1 and y
  * @param alpha Array length 1. The result is stored here. The scaler value for x1
  * @return true if there is a psudeo-inverse, else false
  */
bool pinv(const float *x1, const float *y, int n, double *alpha)
{
  double sum_x1_y=0.0, sum_x1_x1=0.0;
  for(int j=0; j<n; j++) {
    sum_x1_y += x1[j]*y[j];
    sum_x1_x1 += x1[j]*x1[j];
  }
  if(fabs(sum_x1_x1) < MY_EPSILON) return false;
  alpha[0] = sum_x1_y / sum_x1_x1;
  return true;
}

/** Moore-Penrose Matrix-inverse (or pseudo-inverse) such that X*alpha1 - Y is minimised using least squares
  * Ref: http://mathworld.wolfram.com/Moore-PenroseMatrixInverse.html
  * @param x1 The first column of X or NULL for a column of 1s. An array of length n to fit to y
  * @param x2 The second column of X. An array of length n to fit to y
  * @param y The first (and only) column of Y. Y is the matrix to be fitted to, using least squares
  * @param n the number of items in x1 and y
  * @param alpha Array length 2. The result is stored here. The scaler values for x1 and x2 respectively
  * @return true if there is a psudeo-inverse, else false
  */
bool pinv(const float *x1, const float *x2, const float *y, int n, double *alpha)
{
  double s[4], s_inv[4];
  double sum_x1_y=0.0, sum_x2_y=0.0;
  s[0] = s[1] = s[3] = 0.0;
  if(x1) {
    for(int j=0; j<n; j++) {
      s[0] += x1[j]*x1[j];
      s[1] += x1[j]*x2[j];
      s[3] += x2[j]*x2[j];
      sum_x1_y += x1[j]*y[j];
      sum_x2_y += x2[j]*y[j];
    }
  } else {
    s[0] = n;
    for(int j=0; j<n; j++) {
      s[1] += x2[j];
      s[3] += x2[j]*x2[j];
      sum_x1_y += y[j];
      sum_x2_y += x2[j]*y[j];
    }
  }
  s[2] = s[1];
  if(!invert2x2matrix(s, s_inv)) {
    return false;
  }

  alpha[0] = s_inv[0]*sum_x1_y + s_inv[1]*sum_x2_y;
  alpha[1] = s_inv[2]*sum_x1_y + s_inv[3]*sum_x2_y;
  return true;
}

/** Moore-Penrose Matrix-inverse (or pseudo-inverse) such that X*alpha1 - Y is minimised using least squares
  * Ref: http://mathworld.wolfram.com/Moore-PenroseMatrixInverse.html
  * @param x1 The first column of X or NULL for a column of 1s. An array of length n to fit to y
  * @param x2 The second column of X. An array of length n to fit to y
  * @param x3 The third column of X. An array of length n to fit to y
  * @param y The first (and only) column of Y. Y is the matrix to be fitted to, using least squares
  * @param n the number of items in x1 and y
  * @param alpha Array length 3. The result is stored here. The scaler values for x1, x2 and x3 respectively
  * @return true if there is a psudeo-inverse, else false
  */
bool pinv(const float *x1, const float *x2, const float *x3, const float *y, int n, double *alpha)
{
  double s[9], s_inv[9];
  double sum_x1_y=0.0, sum_x2_y=0.0, sum_x3_y=0.0;
  std::fill(s, s+9, 0.0);
  if(x1) {
    for(int j=0; j<n; j++) {
      s[0] += x1[j]*x1[j];
      s[1] += x1[j]*x2[j];
      s[2] += x1[j]*x3[j];
      s[4] += x2[j]*x2[j];
      s[5] += x2[j]*x3[j];
      s[8] += x3[j]*x3[j];
      sum_x1_y += x1[j]*y[j];
      sum_x2_y += x2[j]*y[j];
      sum_x3_y += x3[j]*y[j];
    }
  } else {
    s[0] = n;
    for(int j=0; j<n; j++) {
      s[1] += x2[j];
      s[2] += x3[j];
      s[4] += x2[j]*x2[j];
      s[5] += x2[j]*x3[j];
      s[8] += x3[j]*x3[j];
      sum_x1_y += y[j];
      sum_x2_y += x2[j]*y[j];
      sum_x3_y += x3[j]*y[j];
    }
  }
  s[3] = s[1];
  s[6] = s[2];
  s[7] = s[5];
  if(!invert3x3matrix(s, s_inv)) {
    return false;
  }

  alpha[0] = s_inv[0]*sum_x1_y + s_inv[1]*sum_x2_y + s_inv[2]*sum_x3_y;
  alpha[1] = s_inv[3]*sum_x1_y + s_inv[4]*sum_x2_y + s_inv[5]*sum_x3_y;
  alpha[2] = s_inv[6]*sum_x1_y + s_inv[7]*sum_x2_y + s_inv[8]*sum_x3_y;
  return true;
}
