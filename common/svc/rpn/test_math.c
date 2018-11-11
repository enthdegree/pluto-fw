#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "cordic_wrapped.h"

#define EPS 1.0e-10
const double extremes_set[] = {NAN, -NAN, \
	-INFINITY, -283463*M_PI, \
		-M_PI-EPS, -M_PI, -M_PI+EPS, \
		-3*M_PI/4-EPS, -3*M_PI/4, -3*M_PI/4+EPS, \
		-M_E-EPS, M_E, M_E+EPS, \
		-M_PI/2-EPS, -M_PI/2, -M_PI/2+EPS, \
		-1-EPS, -1, -1+EPS, \
		-1*M_PI/4-EPS, -1*M_PI/4, -1*M_PI/4+EPS, \
		-EPS, -0, 0, EPS, \
		1*M_PI/4-EPS, 1*M_PI/4, 1*M_PI/4+EPS, \
		1-EPS, 1, 1+EPS, \
		M_PI/2-EPS, M_PI/2, M_PI+EPS, \
		M_E-EPS, M_E, M_E+EPS, \
		3*M_PI/4-EPS, 3*M_PI/4, 3*M_PI/4+EPS, \
		M_PI-EPS, M_PI, M_PI+EPS, \
		19837*M_PI, INFINITY};
int n_test = sizeof(extremes_set)/sizeof(double);
typedef double (*unary_ptr)(double);

/* diff: Return a modified difference between two unary double operators at a point
 * Modified as so: If the results are both NAN or INFINITY or -INFINITY return 0.0. 
 * If one result is NAN return INFINITY.
 *
 * Inputs: 
 *  unary_ptr a = function a
 *  unary_ptr b = function b
 *  doule pt = point
 *
 * Output: Modified difference
 */
double diff( unary_ptr a, unary_ptr b, double pt) {
	double a_pt = a(pt);
	double b_pt = b(pt);
	if( (isnan(a_pt) & isnan(b_pt)) || (a_pt == b_pt) ) return 0.0; 
	if( isnan(a_pt) || isnan(b_pt) ) return INFINITY;
	return abs(a(pt)-b(pt));
}
#define TEST_UNARY_EXTREMES(A,B) \
	printf("testing "#A" against "#B"...\n");\
	for(int idx=0; idx<n_test; idx++) {\
		double err = diff(A,B, extremes_set[idx]);\
		double badval = A(extremes_set[idx]);\
		double goodval = B(extremes_set[idx]);\
		if(abs(err)/goodval > EPS) {\
			printf("\033[1m\033[31m"#A"(%0.12g) = %0.12g, ", extremes_set[idx], badval);\
			printf(#B"(%0.12g) = %0.12g\033[0m\n", extremes_set[idx], goodval);\
		}\
	}

int main() {
	TEST_UNARY_EXTREMES(sin_wc,sin)
	TEST_UNARY_EXTREMES(cos_wc,cos)
	TEST_UNARY_EXTREMES(tan_wc,tan)

	TEST_UNARY_EXTREMES(log_wc,log)
	TEST_UNARY_EXTREMES(sqrt_wc,sqrt)
	TEST_UNARY_EXTREMES(exp_wc,exp)

	TEST_UNARY_EXTREMES(asin_wc,asin)
	TEST_UNARY_EXTREMES(acos_wc,acos)
	TEST_UNARY_EXTREMES(atan_wc,atan)

	TEST_UNARY_EXTREMES(sinh_wc,sinh)
	TEST_UNARY_EXTREMES(cosh_wc,cosh)
	TEST_UNARY_EXTREMES(tanh_wc,tanh)

	TEST_UNARY_EXTREMES(asinh_wc,asinh)
	TEST_UNARY_EXTREMES(acosh_wc,acosh)
	TEST_UNARY_EXTREMES(atanh_wc,atanh)

	return 0;
}
