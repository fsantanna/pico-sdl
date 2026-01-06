# Makefile for pico-sdl tests

.PHONY: tests clean

tests:
	@echo "Running tests..."
	@mkdir -p tst/gen

	./pico-sdl tst/cv.c
	./pico-sdl tst/vs.c
	./pico-sdl tst/anchor_pct.c

	@echo ""
	@echo "All tests passed!"

clean:
	rm -f tst/*.exe
	rm -f tst/gen/*.png
