LIBS   = -lSDL2 # -lkernel32 -luser32 -lgdi32 -lopengl32
CFLAGS = -Wall #-Wno-switch #-Wno-return-local-addr

SRC=$(wildcard src/*.c)

main: $(SRC) src/main/main.c
	gcc -g -o $@ $^ $(CFLAGS) $(LIBS)
