#include <errno.h>
#include <string.h>

#define E(a,b) ((unsigned char)a),
static const unsigned char errid[] = {
#include "__strerror.h"
};

#undef E
#define E(a,b) b "\0"
static char *errmsg[] = {
#include "__strerror.h"
};

char *strerror(int e) {
  int i;
  for(i=0; errid[i] && (e != errid[i]); i++);
  return errmsg[i];
}
