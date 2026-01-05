# Makefile for pico-sdl tests

.PHONY: tests clean

tests:
	@echo "Running tests..."

	./pico-sdl tst/cv.c
	./pico-sdl tst/vs.c

	@echo ""
	@echo "All tests passed!"

todo:
	@mkdir -p tst/output
	@echo "vs..."
	@gcc -Wall -g -o tst/vs.exe tst/vs.c src/pico.c src/hash.c -Isrc -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx
	@cd tst && SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ./vs.exe

	@echo "anchor_pct..."
	@gcc -Wall -g -o tst/anchor_pct.exe tst/anchor_pct.c src/pico.c src/hash.c -Isrc -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx
	@cd tst && SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ./anchor_pct.exe


clean:
	rm -f tst/*.exe
	rm -f tst/output/*.png
