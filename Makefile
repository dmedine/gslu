CC=gcc

SRCS=$(PROG).c
SRCS+=shader_utils.c gsl_utils.c
OBJS=$(SRCS:.c=.o) 

LIBS=-lglut -lGLEW -lGL -lm -lgsl
LIBPATH=-L/usr/local/ -Llib -L/lib64

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -g -o $@ $^ $(LIBPATH) $(LIBS)

clean:
	rm -f *.o $(PROG)

.PHONY: all clean $(PROG)
