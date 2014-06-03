#ifndef _FAST_SMOOTH_H
#define _FAST_SMOOTH_H

//rotates a the complex number (sin_sum, cos_sum) by an angle.
inline void fast_complex_rotate(double &x, double &y, double sin_angle, double cos_angle)
{
    double temp = y*cos_angle - x*sin_angle;
    x = y*sin_angle + x*cos_angle;
    y = temp;
}

/*A class to perform smoothing/bluring on data using a hanning (cos shaped) window.
This uses and fast internal rotation algorithm*/
class fast_smooth
{
    int _size, _size_left, _size_right;
    double _angle;
    double _cos_angle, _sin_angle;
    double _sum;

 public:
    fast_smooth(int size); //the total width of the hanning window. To keep data centered use an odd size
    void fast_smoothA(float *source, float *dest, int length, int step);
    void fast_smoothA(float *source, float *dest, int length);
    void fast_smoothB(float *source, float *dest, int length, int step);
    void fast_smoothB(float *source, float *dest, int length);
};

void testFastSmooth();

#endif
