/* Wrapped calls to voidware CORDIC functions (handles domain issues)
 */

#include <math.h>
#include "cordic.h"
#define MYNAN (0.0/0.0)
#define PLUS_INFTY (1.0/0.0)
#define NEG_INFTY (-1.0/0.0)

/** Linear features ***********************************************/
double mul_wc(double a, double b) {
	return mulCordic(a,b);
}
double div_wc(double a, double b) {
	return divCordic(a,b);
}

/** circular features ***********************************************/
double sin_wc(double a) {
	int shift = (int) floor(a/(M_PI/2.0))%4;
	double b, trash, res;
	b = a - shift*(M_PI/2.0);

	if((shift == 0)||(shift == 2)) res = sincosCordic(b, &trash);
	else res = sincosCordic(M_PI/2.0 - b, &trash);

	if((shift == 0)||(shift==1)) return res;
	else return -res;
}
double cos_wc(double a) {
	int shift = (int) floor(a/(M_PI/2.0))%4;
	double b, res;
	b = a - shift*(M_PI/2.0);

	if((shift == 0)||(shift == 2)) sincosCordic(b, &res);
	else sincosCordic(M_PI/2.0 - b, &res);

	if((shift == 0)||(shift==1)) return res;
	else return -res;
}
double tan_wc(double a) {
	int shift = (int) floor(a/(M_PI/2.0))%4;
	double b, res;
	b = a - shift*(M_PI/2.0);

	if((shift == 0)||(shift == 2)) res = tanCordic(b);
	else res = tanCordic(M_PI/2.0 - b);

	if((shift == 0)||(shift==1)) return res;
	else return -res;
}

double asin_wc(double a) {
	if(a >= 0.98) return PLUS_INFTY;
	else if(a <= -0.98) return NEG_INFTY;
	else return asinCordic(a);
}

double atan_wc(double a) {
	return atanCordic(a);
}

/** hyperbolic features ********************************************/
double dipow(double base, int exp)
{
    if(exp<0) { 
	    base = 1.0/base;
	    exp = -exp;
    }
    double result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

double exp_wc(double a) {
	double ii;
	double dd = modf(a, &ii);
	return expCordic(dd)*dipow(M_E, (int) ii);
}

double sinh_wc(double a) {
	if((a<=1)&&(a>=-1)) {
		double trash;
		return sinhcoshCordic(a, &trash);
	}
	else {
		double ii;
		double dd = modf(a, &ii);
		return (exp_wc(dd)*dipow(M_E, (int) ii) - exp_wc(-dd)*dipow(1.0/M_E, (int) ii))/2.0;
	}
}

double cosh_wc(double a) {
	if((a<=1)&&(a>=-1)) {
		double res;
		sinhcoshCordic(a, &res);
		return res;
	}
	else {
		double ii;
		double dd = modf(a, &ii);
		return (exp_wc(dd)*dipow(M_E, (int) ii) + exp_wc(-dd)*dipow(1.0/M_E, (int) ii))/2.0;
	}
}

double tanh_wc(double a) {
	if((a<=1)&&(a>=-1)) return tanCordic(a);
	else return sinh_wc(a)/cosh_wc(a);
}

double atanh_wc(double a) {
	if((a<=1)&&(a>=-1)) return atanhCordic(a);
	else return MYNAN;
}

double log_wc(double a) {
	if(a<=0) return MYNAN;
	else if(a <= 0.1) {
		int shift = 0;
		double rem = a;
		while(rem < 0.1) {
			rem = rem*M_E;
			shift++;
		}
		return logCordic(a*dipow(M_E,shift))-shift;
	}
	else if(a>9) {
		int shift = 0;
		double rem = a;
		while(rem > 9) {
			rem = rem/M_E;
			shift++;	
		}
		return logCordic(a*dipow(1.0/M_E,shift))+shift;
	}
	else return logCordic(a);
}

double sqrt_wc(double a) {
	if(a<=0) return MYNAN;
	else if(a<=0.03) {
		int shift = 0;
		double rem = a;
		while(rem <= 0.03) {
			shift++;	
			rem = rem*2;
		}
		// a = 2^(-shift)*rem
		if(0 == (shift%2)) return sqrtCordic(rem)/dipow(2.0,shift/2);
		else return (sqrtCordic(rem)/dipow(2.0,shift/2))/M_SQRT2;
	}
	else if(a>=2) {
		int shift = 0;
		double rem = a;
		while(rem > 9) {
			shift++;	
			rem = rem/2;
		}
		// a = 2^(shift)*rem
		if(0 == (shift%2)) return dipow(2.0,shift/2)*sqrtCordic(rem);
		else return M_SQRT2*dipow(2.0,shift/2)*sqrtCordic(rem);
	}
	else return sqrtCordic(a);
}

// Functions that only depend on the above
double acos_wc(double a) {
	return asinCordic(sqrt_wc(1-a*a));
}
double asinh_wc(double a) {
	return log_wc(a+sqrt_wc(a*a+1));
}
double acosh_wc(double a) {
	return log_wc(a+sqrt_wc(a*a-1));
}
