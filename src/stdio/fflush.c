#include <errno.h>
#include <barelibc/stdio.h>


int fflush(FILE *stream) {
  if (!stream)
    stream = stdout;

  if (!stream->write) {
    errno = EBADF;
    return -1;
  }

  stream->write(stream, NULL, 0);
  return 0;
}
