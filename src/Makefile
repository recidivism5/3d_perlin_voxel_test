IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR) -IC:\Users\destr\Documents\coding\C\Libs\SDL2-2.0.14\x86_64-w64-mingw32\include\ -IC:\Users\destr\Documents\coding\C\Libs\glew-2.1.0\include

ODIR=obj
LDIR =../lib

BIN = ../bin

LIBS=-lm -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32

_DEPS = pipeline.h shader.h window.h cube.h quaternion.h matrices.h texture.h world.h mtwister.h hash.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o pipeline.o shader.o window.o cube.o quaternion.o matrices.o texture.o world.o mtwister.o hash.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $(BIN)/$@ $^ $(CFLAGS) -LC:\Users\destr\Documents\coding\C\Libs\SDL2-2.0.14\x86_64-w64-mingw32\lib -LC:\Users\destr\Documents\coding\C\Libs\glew-2.1.0\lib $(LIBS)

.PHONY: clean

clean:
	del -f $(ODIR)/*.o *~ core $(INCDIR)/*~
