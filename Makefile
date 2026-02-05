CHECK = PICO_TESTS=1 PICO_CHECK_INT= PICO_CHECK_ASR=1
XVFB = xvfb-run -a
EXE = $(CHECK) $(XVFB) ./pico-sdl
INT = PICO_TESTS=1 PICO_CHECK_INT=1 PICO_CHECK_ASR= ./pico-sdl
#EXE = PICO_TESTS=1 PICO_CHECK_INT= PICO_CHECK_ASR= xvfb-run -a ./pico-sdl

.PHONY: ttl tests clean

all: tests

ttl:
	curl -sL https://raw.githubusercontent.com/fsantanna/ttl-hash/v0.1/ttl_hash.h \
		-o src/hash.h

test:
	$(EXE) tst/$(T).c

int:
	$(INT) tst/$(T).c

tests:
	@echo "Running tests..."
	$(EXE) tst/init.c
	$(EXE) tst/cv.c
	$(EXE) tst/vs.c
	$(EXE) tst/anchor_raw.c
	$(EXE) tst/anchor_pct.c
	$(EXE) tst/blend_raw.c
	$(EXE) tst/blend_pct.c
	$(EXE) tst/buffer_raw.c
	$(EXE) tst/buffer_pct.c
	$(EXE) tst/expert.c
	$(EXE) tst/clip_raw.c
	$(EXE) tst/clip_pct.c
	$(EXE) tst/collide_raw.c
	$(EXE) tst/collide_pct.c
	$(EXE) tst/colors.c
	$(EXE) tst/cross.c
	$(EXE) tst/dim.c
	$(EXE) tst/font.c
	$(EXE) tst/image_raw.c
	$(EXE) tst/image_pct.c
	$(EXE) tst/layers.c
	$(EXE) tst/mouse.c
	$(EXE) tst/move.c
	$(EXE) tst/navigate.c
	$(EXE) tst/pixels.c
	$(EXE) tst/pos.c
	$(EXE) tst/quit.c
	$(EXE) tst/rect.c
	$(EXE) tst/shot.c
	$(EXE) tst/size_raw.c
	$(EXE) tst/size_pct.c
	$(EXE) tst/tiles.c
	$(EXE) tst/view_raw.c
	$(EXE) tst/push.c
	@echo ""
	@echo "All tests passed!"

clean:
	rm -f tst/*.exe
	rm -f tst/out/*.png
