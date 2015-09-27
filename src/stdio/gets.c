#include <limits.h>
#include <string.h>
#include <barelibc/stdio.h>


char *gets(char *s)
{
	char *ret = fgets(s, INT_MAX, stdin);
	if (ret && s[strlen(s)-1] == '\n') s[strlen(s)-1] = 0;
	return ret;
}
