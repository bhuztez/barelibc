#include <unistd.h> 

#include <barelibc/stdio.h>


static size_t stderr_write(FILE * file __attribute__((unused)), const unsigned char * s, size_t n) {
  size_t count = 0;

  if (s) {
    count += write(2, s, n);
  }

  return count;
}

static FILE __stderr = {
  .read = NULL,
  .write = stderr_write,
  .buf = NULL,
  .pos = NULL,
  .end = NULL,
  .eof = 0,
};

FILE *stderr = &__stderr;
