# Makefile for pico-sdl tests

.PHONY: tests clean

tests:
	@echo "Running tests..."
	@mkdir -p tst/output

	@echo "anchor_pct..."
	@gcc -Wall -g -o tst/anchor_pct.exe tst/anchor_pct.c src/pico.c src/hash.c -Isrc -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx
	@cd tst && SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ./anchor_pct.exe

	@echo "cv..."
	@gcc -Wall -g -o tst/cv.exe tst/cv.c src/pico.c src/hash.c -Isrc -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx
	@cd tst && SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ./cv.exe

	@echo "vs..."
	@gcc -Wall -g -o tst/vs.exe tst/vs.c src/pico.c src/hash.c -Isrc -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx
	@cd tst && SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ./vs.exe

	@rm tst/*.exe
	@echo ""
	@echo "All tests passed!"

clean:
	rm -f tst/*.exe
	rm -f tst/output/*.png
