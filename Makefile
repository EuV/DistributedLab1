PROG = pa1

CC = gcc
CFLAGS  += -Wall --pedantic -std=c99

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

.PHONY : all
all: depend $(PROG)

$(PROG): $(OBJS)

.PHONY : clean
clean:
	-rm -f *.o \
		*.log \
		.depend \
		$(PROG)

.PHONY : depend
depend: .depend
.depend: $(SRCS)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend;

include .depend
