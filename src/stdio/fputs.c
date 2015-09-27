#include <string.h>

#include <barelibc/stdio.h>


int fputs(const char *s, FILE *stream) {
  return (int)fwrite(s, strlen(s), 1, stream) - 1;
}
