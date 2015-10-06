CC=gcc
AR=ar
RANLIB=ranlib
ARCH=x86_64
CFLAGS = -Os -pipe -fomit-frame-pointer -fno-unwind-tables -fno-asynchronous-unwind-tables -Wa,--noexecstack -Wall -Wextra -Werror -I./include
CFLAGS_C99FSE = -nostdlib -std=gnu11 -ffreestanding -fexcess-precision=standard -frounding-math
CFLAGS_MEMOPS = -fno-tree-loop-distribute-patterns
CFLAGS_NOSSP = -fno-stack-protector
CPPFLAGS =
LDFLAGS = -Wl,--hash-style=both
LIBCC = -lgcc -lgcc_eh
CFLAGS_ALL = $(CFLAGS_C99FSE) -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700 $(CPPFLAGS) $(CFLAGS)

EMPTY_LIB_NAMES = m rt pthread crypt util xnet resolv dl
EMPTY_LIBS = $(EMPTY_LIB_NAMES:%=lib/lib%.a)

CRT_SRCS = $(sort $(wildcard crt/$(ARCH)/*.s))
CRT_LIBS = $(CRT_SRCS:crt/$(ARCH)/%.s=lib/%.o)

ASMS = $(sort $(wildcard src/*/$(ARCH)/*.s))
SRCS = $(sort $(wildcard src/*/*.c))
OBJS = $(SRCS:src/%.c=obj/%.o) $(ASMS:src/%.s=obj/%.o)

STATIC_LIBS = lib/libc.a
ALL_LIBS = $(CRT_LIBS) $(STATIC_LIBS) $(EMPTY_LIBS)

DIRS = $(sort $(foreach obj,$(OBJS) $(ALL_LIBS),$(dir $(obj))))

OPTIMIZE_GLOBS = internal/*.c malloc/*.c string/*.c
OPTIMIZE_SRCS = $(wildcard $(OPTIMIZE_GLOBS:%=src/%))
$(OPTIMIZE_SRCS:src/%.c=obj/%.o): CFLAGS += -O3

MEMOPS_SRCS = src/string/memcpy.c src/string/memmove.c src/string/memcmp.c src/string/memset.c
$(MEMOPS_SRCS:src/%.c=obj/%.o): CFLAGS += $(CFLAGS_MEMOPS)

NOSSP_SRCS = \
	src/env/__libc_start_main.c \
	src/string/memset.c src/string/memcpy.c
$(NOSSP_SRCS:src/%.c=obj/%.o): CFLAGS += $(CFLAGS_NOSSP)


CFLAGS_COMPLEX = -Wno-unknown-pragmas
COMPLEX_SRCS = $(wildcard src/complex/*.c)
$(COMPLEX_SRCS:src/%.c=obj/%.o): CFLAGS += $(CFLAGS_COMPLEX)

CFLAGS_MATH = -Wno-parentheses -Wno-unused-but-set-variable -Wno-unknown-pragmas -Wno-maybe-uninitialized -Wno-unused-parameter -Wno-sign-compare
MATH_SRCS = $(wildcard src/math/*.c)
$(MATH_SRCS:src/%.c=obj/%.o): CFLAGS += $(CFLAGS_MATH)

CFLAGS_INTERNAL = -Wno-parentheses -Wno-sign-compare
INTERNAL_SRCS = $(wildcard src/internal/*.c src/string/*.c src/stdio/*.c src/stdlib/*.c)
$(INTERNAL_SRCS:src/%.c=obj/%.o): CFLAGS += $(CFLAGS_INTERNAL)

all: $(ALL_LIBS)

clean:
	$(RM) -rf lib obj

lib/%.o: crt/$(ARCH)/%.s | $(DIRS)
	$(CC) -c -o "$@" "$<"

obj/%.o: src/%.s | $(DIRS)
	$(CC) -c -o "$@" "$<"

obj/%.o: src/%.c | $(DIRS)
	$(CC) $(CFLAGS_ALL) -c -o "$@" "$<"

lib/libc.a: $(OBJS) | $(DIRS)
	$(AR) rc "$@" $(OBJS)
	$(RANLIB) "$@"

$(EMPTY_LIBS):
	$(AR) rc "$@"

$(DIRS): %:
	mkdir -p "$@"

.PHONY: all clean
