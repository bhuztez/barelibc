#include <unistd.h>

#include <barelibc/syscall.h>

ssize_t read(int fd, void *buf, size_t count) {
  return __syscall(SYS_read, fd, buf, count);
}
