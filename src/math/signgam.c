#include <math.h>
#include <barelibc/libc.h>

int __signgam = 0;

weak_alias(__signgam, signgam);
