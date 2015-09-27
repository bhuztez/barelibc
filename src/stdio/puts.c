#include <barelibc/stdio.h>

int puts(const char *s)
{
	return -(fputs(s, stdout) < 0 || fputc('\n', stdout) < 0);
}
