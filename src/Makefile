IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR =../lib

BIN = ../bin

LIBS=-lm -lSDL2main -lSDL2 -lGLEW -lGL

_DEPS = pipeline.h shader.h window.h cube.h quaternion.h matrices.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o pipeline.o shader.o window.o cube.o quaternion.o matrices.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

persp: $(OBJ)
	$(CC) -o $(BIN)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
