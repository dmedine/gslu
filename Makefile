#gslu/Makefile

CC=gcc

SRCS=cube.c
SRCS+=shader_utils.c 
OBJS=$(SRCS:.c=.o) 

CFLAGS=-I./lib

LIBS=-lglut -lGLEW -lGL -lm -lgsl -lgslu
LIBPATH=-L/usr/local/ -L/lib -L/lib64 -L./lib

.PHONY: lib cube

all: lib cube

lib:
	$(MAKE) -C lib

cube: $(OBJS)
	$(CC) -g -o $@ $^ $(CFLAGS) $(LIBPATH) $(LIBS)

clean:
	rm -f *.o 
	rm  gslu/libgslu.a

install: 
	mkdir -p /usr/local/include/GSLU
	cp lib/gsl_utils.h /usr/local/include/GSLU/gsl_utils.h
	cp lib/libgslu.a /usr/local/lib/libgslu.a
	cp lib/gslu.pc /usr/share/pkgconfig/gslu.pc
