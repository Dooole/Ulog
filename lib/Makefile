# Makefile template for a shared library in C
# https://www.topbug.net/blog/2019/10/28/makefile-template-for-a-shared-library-in-c-with-explanations/

CFLAGS += -fPIC -Wall -Wextra -O2 -g  # C flags
LDFLAGS += -shared -lsqlite3   # linking flags
RM = rm -f   # rm command
INSTALL = install
TARGET_LIB = libulog.so  # target lib

SRCS = main.c   #source files
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${TARGET_LIB}

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)

.PHONY: install
install:
	$(INSTALL) -m 755 ${TARGET_LIB} /usr/lib/${TARGET_LIB}
	$(INSTALL) -m 644 ulog.h /usr/include/ulog.h

.PHONY: uninstall
uninstall:
	$(RM) /usr/lib/${TARGET_LIB}
	$(RM) /usr/include/ulog.h

$(TARGET_LIB): $(OBJS)
	$(CC) -o $@ $^ ${LDFLAGS}

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c=.d)
