CHECK = PICO_TESTS=1 PICO_CHECK_INT= PICO_CHECK_ASR=1
XVFB = SDL_AUDIODRIVER=dummy xvfb-run -a
EXE = $(CHECK) $(XVFB) ./pico-sdl
INT = PICO_TESTS=1 PICO_CHECK_INT=1 PICO_CHECK_ASR= ./pico-sdl
GEN = PICO_TESTS=1 PICO_CHECK_INT= PICO_CHECK_ASR= xvfb-run -a ./pico-sdl

.PHONY: realm tests lua clean

all: tests

realm:
	curl -sL https://raw.githubusercontent.com/fsantanna/realm-allocator/v0.1/realm.h \
		-o src/realm.hc

lua:
	sudo luarocks make lua/pico-sdl-0.3.1-1.rockspec --lua-version=5.4

test:
	$(EXE) tst/$(T).c

int:
	$(INT) tst/$(T).c

gen:
	$(GEN) tst/$(T).c
	cp tst/out/$(T)-*.png tst/asr/

tests:
	@echo "Running tests..."
	./pico-sdl | grep -q "Usage: pico-sdl"
	./pico-sdl --help | grep -q "Usage: pico-sdl"
	./pico-sdl --version | grep -q "v0.4-dev"
	$(EXE) tst/anchor_pct.c
	$(EXE) tst/anchor_raw.c
	$(EXE) tst/blend_pct.c
	$(EXE) tst/blend_raw.c
	$(EXE) tst/buffer_pct.c
	$(EXE) tst/buffer_raw.c
	$(EXE) tst/clear_alpha.c
	$(EXE) tst/clip_pct.c
	$(EXE) tst/clip_raw.c
	$(EXE) tst/collide_pct.c
	$(EXE) tst/collide_raw.c
	$(EXE) tst/colors.c
	$(EXE) tst/cross.c
	$(EXE) tst/cv.c
	$(EXE) tst/dim.c
	$(EXE) tst/expert.c
	$(EXE) tst/expert_fps.c
	$(EXE) tst/font.c
	$(EXE) tst/get-set.c
	$(EXE) tst/image_pct.c
	$(EXE) tst/image_raw.c
	$(EXE) tst/init.c
	$(EXE) tst/keep.c
	$(EXE) tst/layer-empty-tile.c
	$(EXE) tst/layer-hier.c
	$(EXE) tst/layers.c
	$(EXE) tst/mouse.c
	$(EXE) tst/mouse-rect-click.c
	$(EXE) tst/mouse-w-click.c
	$(EXE) tst/move.c
	$(EXE) tst/navigate.c
	$(EXE) tst/pixels.c
	$(EXE) tst/pos.c
	$(EXE) tst/quit.c
	$(EXE) tst/rect.c
	$(EXE) tst/rot-flip.c
	$(EXE) tst/sheet.c
	$(EXE) tst/shot.c
	$(EXE) tst/size_pct.c
	$(EXE) tst/size_raw.c
	$(EXE) tst/style.c
	$(EXE) tst/text_empty.c
	$(EXE) tst/tile-grid.c
	$(EXE) tst/tiles.c
	$(EXE) tst/video.c
	$(EXE) tst/view-target.c
	$(EXE) tst/view_raw.c
	$(EXE) tst/vs.c
	@echo ""
	@echo "All tests passed!"

clean:
	rm -f tst/*.exe
	rm -f tst/out/*.png
