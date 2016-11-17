SRCDIR=Src
TESTDIR=Tst
VPATH=$(SRCDIR):$(TESTDIR)

CC=gcc
CFLAGS= -std=c11 -O3 -Wall -march=native -I$(SRCDIR) -I$(TESTDIR) -DMKL_ILP64 -fopenmp -I${MKLROOT}/include `sdl2-config --cflags`
LDFLAGS= -Wall -L$(SRCDIR) -L$(TESTDIR)`sdl2-config --libs` -lSDL2_gfx -fopenmp -L${MKLROOT}/lib/intel64 -lmkl_intel_ilp64 -lmkl_core -lmkl_gnu_thread -lpthread -lm -ldl

SRC=template.c
OBJ=$(SRC:.c=.o)

EXEC=tests

all:$(EXEC)

tests: tests.o $(OBJ)
	$(CC) -o $@ $^ ${LDFLAGS}
	rm -rf *.o

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
