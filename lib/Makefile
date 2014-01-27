#gslu/lib/Makefile

CC=gcc

CFLAGS  = -g -o 
CFLAGS += -ffreestanding

LIBPATH = -L/usr/lib -L/lib -L/lib64 -L/usr/include 
LIBS = -lm -lgsl

SRC = gsl_utils.c

OBJ = $(SRC:.c=.o)

.PHONY: libgslu.a

all: libgslu.a

%.o: %.c
	$(CC) $(CFLAGS) $(LIBPATH) $(LIBS)  -c -o $@ $^

libgslu.a: $(OBJ) 
	$(AR) -r $@ $(OBJ) 
	#mv *.o $(OBJ_PATH)
	#mv libgslu.a ../
clean:
	rm *.o
	rm libgslu.a
