# Makefile for pico-sdl tests

EXE = PICO_CHECK_INT= PICO_CHECK_ASR=1 xvfb-run -a ./pico-sdl

.PHONY: tests clean

tests:
	@echo "Running tests..."

	$(EXE) tst/cv.c
	$(EXE) tst/vs.c
	$(EXE) tst/anchor_pct.c
	$(EXE) tst/blend_raw.c
	$(EXE) tst/blend_pct.c
	$(EXE) tst/buffer_raw.c
	$(EXE) tst/buffer_pct.c

	@echo ""
	@echo "All tests passed!"

clean:
	rm -f tst/*.exe
	rm -f tst/out/*.png
