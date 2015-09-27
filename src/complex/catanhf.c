#include <barelibc/libm.h>

float complex catanhf(float complex z)
{
	z = catanf(CMPLXF(-cimagf(z), crealf(z)));
	return CMPLXF(cimagf(z), -crealf(z));
}
