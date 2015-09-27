#include <errno.h>
#include <barelibc/stdio.h>

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  if (!stream->read) {
    errno = EBADF;
    return -1;
  }

  return stream->read(stream, ptr, size*nmemb)/size;
}
