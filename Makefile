LIBS   = -lSDL2 -lSDL2_gfx -lSDL2_image -lSDL2_ttf -lSDL2_mixer # -lkernel32 -luser32 -lgdi32 -lopengl32
CFLAGS = -Wall #-Wno-switch #-Wno-return-local-addr

ALL=$(wildcard src/*.c)

main: $(ALL) src/main/main.c
	gcc -g -o $@ $^ $(CFLAGS) $(LIBS)

src: $(ALL) src/main/$(SRC).c
	gcc -g -o $(SRC) $^ $(CFLAGS) $(LIBS)

.PHONY: main src
