// Taken from http://www.voidware.com/cordic.htm 

#include <math.h>

#include "cordic.h"
#include "cordic_consts.c"

void cordit1(double* x0, double* y0, double* z0, double vecmode)
{
    /* this is the circular method. 
     * one slight change from the other methods is the y < vecmode 
     * test. this is to implement arcsin, otherwise it can be
     * y < 0 and you can compute arcsin from arctan using
     * trig identities, so its not essential.
     */
    double t;
    double x, y, z;
    int i;
    t = 1;
    x = *x0; y = *y0; z = *z0;
    for (i = 0; i < MAXBITS; ++i) {
        double x1;
        if (vecmode >= 0 && y < vecmode || vecmode<0  && z >= 0) {
            x1 = x - y*t;
            y = y + x*t;
            z = z - atanTable[i];
        }
        else {
            x1 = x + y*t;
            y = y - x*t;
            z = z + atanTable[i];
        }
        x = x1;
        t /= 2;
    }
    *x0 = x;
    *y0 = y;
    *z0 = z;
}
void cordit2(double* x0, double* y0, double* z0, double vecmode)
{
    /* here's the hyperbolic methods. its very similar to
     * the circular methods, but with some small differences.
     *
     * the `x' iteration have the opposite sign.
     * iterations 4, 7, .. 3k+1 are repeated.
     * iteration 0 is not performed.
     */
    double t;
    double x, y, z;
    int i;
    t = 0.5;
    x = *x0; y = *y0; z = *z0;
    int k = 3;
    for (i = 0; i < MAXBITS; ++i) {
        double x1;
        int j;
        for (j = 0; j < 2; ++j) {
            if (vecmode >= 0 && y < 0 || vecmode<0  && z >= 0) {
                x1 = x + y*t;
                y = y + x*t;
                z = z - atanhTable[i];
            }
            else {
                x1 = x - y*t;
                y = y - x*t;
                z = z + atanhTable[i];
            }
            x = x1;
            if (k) {
                --k;
                break;
            }
            else k = 3;
        }
        t /= 2;
    }
    *x0 = x;
    *y0 = y;
    *z0 = z;
}
void cordit0(double* x0, double* y0, double* z0, double vecmode)
{
    /* the linear methods is the same as the circular but
     * ive simplified out the x iteration as it doesnt change.
     */
    double t;
    double x, y, z;
    int i;
    t = 1;
    x = *x0; y = *y0; z = *z0;
    for (i = 0; i < MAXBITS; ++i) {
        if (vecmode >= 0 && y < 0 || vecmode<0  && z >= 0) {
            y = y + x*t;
            z = z - t;
        }
        else {
            y = y - x*t;
            z = z + t;
        }
        t /= 2;
    }
    *x0 = x;
    *y0 = y;
    *z0 = z;
}
/** Linear features ***********************************************/
double mulCordic(double a, double b)
{
    double x, y, z;
    x = a;
    y = 0;
    z = b;
    cordit0(&x, &y, &z, -1);
    return y;
}
double divCordic(double a, double b)
{
    double x, y, z;
    x = b;
    y = a;
    z = 0;
    cordit0(&x, &y, &z, 0);
    return z;
}

/** circular features ***********************************************/
double atanCordic(double a)
{
    /* domain: all a */
    double x = 1;
    double z = 0;
    cordit1(&x, &a, &z, 0);
    return z;
}
double sincosCordic(double a, double* cosp)
{
    /* |a| < 1.74 */
    double sinp = 0;
    *cosp = invGain1;
    cordit1(cosp, &sinp, &a, -1);
    return sinp;
}
double tanCordic(double a)
{
    /* |a| < 1.74 */
    double sinp = 0;
    double cosp = invGain1;
    cordit1(&cosp, &sinp, &a, -1);
    return DIVD(sinp, cosp);
}
double asinCordic(double a)
{
    /* |a| < 0.98 */
    double x, y, z;
    
    x = invGain1;
    y = 0;
    z = 0;
    int neg = 1;
    if (a < 0) {
        a = -a;
        neg = 0;
    }
        
    cordit1(&x, &y, &z, a);
    if (neg) z = -z;
    return z;
}

/** hyperbolic features ********************************************/
double sinhcoshCordic(double a, double* coshp)
{
    /* |a| < 1.13 */
    double y;
    *coshp = invGain2;
    y = 0;
    cordit2(coshp, &y, &a, -1);
    return y;
}
double tanhCordic(double a)
{
    /* |a| < 1.13 */
   double sinhp, coshp;
   coshp = invGain2;
   sinhp = 0;
   cordit2(&coshp, &sinhp, &a, -1);
   return DIVD(sinhp,coshp);
}
double atanhCordic(double a)
{
    /* |a| < 1.13 */
    double x, z;
    x = 1;
    z = 0;
    cordit2(&x, &a, &z, 0);
    return z;
}
double logCordic(double a)
{
    /* 0.1 < a < 9.58 */
    double x, y, z;
    x = a + 1;
    y = a - 1;
    z = 0;
    cordit2(&x, &y, &z, 0);
    return 2*z;
}
double sqrtCordic(double a)
{
    /* 0.03 < a < 2 */
    double x, y, z;
    x = a + 0.25;
    y = a - 0.25;
    z = 0;
    cordit2(&x, &y, &z, 0);
    return MULT(x, invGain2);
}
double expCordic(double a)
{
    double sinhp, coshp;
    coshp = invGain2;
    sinhp = 0;
    cordit2(&coshp, &sinhp, &a, -1);
    return sinhp + coshp;
}

