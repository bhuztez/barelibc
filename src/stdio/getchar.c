#include <barelibc/stdio.h>

int getchar() {
  return fgetc(stdin);
}
