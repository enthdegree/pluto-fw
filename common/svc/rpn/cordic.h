// Taken from http://www.voidware.com/cordic.htm 

/** Linear features ***********************************************/
double mulCordic(double a, double b);
double divCordic(double a, double b);

/* define this to perform all (non 2power) multiplications
 * and divisions with the cordic linear method.
 */
#ifdef CORDIC_LINEAR
#define MULT(_a, _b)    mulCordic(_a, _b)
#define DIVD(_a, _b)    divCordic(_a, _b)
#else
#define MULT(_a, _b)    (_a)*(_b)
#define DIVD(_a, _b)    (_a)/(_b)
#endif 

/** circular features ***********************************************/
double atanCordic(double a);
double sincosCordic(double a, double* cosp);
double tanCordic(double a);
double asinCordic(double a);

/** hyperbolic features ********************************************/
double sinhcoshCordic(double a, double* coshp);
double tanhCordic(double a);
double atanhCordic(double a);
double logCordic(double a);
double sqrtCordic(double a);
double expCordic(double a);

