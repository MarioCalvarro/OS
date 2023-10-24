SRC = $(wildcard *.c)
TARGETS = $(SRC:%.c=%)
HEADERS = $(wildcard *.h)

CC = gcc
CFLAGS = -g
LDFLAGS =
LIBS =

all: $(TARGETS)

%.o: %.c $(HEADERS) Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGETS): %: %.o
	$(CC) -o $@.x $^ $(LIBS)

.PHONY: clean

clean:
	-rm $(TARGETS) $(SRC:%.c=%.o)

