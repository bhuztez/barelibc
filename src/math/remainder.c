#include <math.h>
#include <barelibc/libc.h>

double remainder(double x, double y)
{
	int q;
	return remquo(x, y, &q);
}

weak_alias(remainder, drem);
