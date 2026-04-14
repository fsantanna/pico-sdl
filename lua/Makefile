XVFB = SDL_AUDIODRIVER=dummy xvfb-run -a
LUA = LUA_CPATH="../pico/?.so;" LUA_PATH="../?/init.lua;../?.lua;"
TST = PICO_CHECK_ASR=1 $(LUA) $(XVFB) ./pico-lua
INT = PICO_CHECK_INT=1 PICO_CHECK_ASR= $(LUA) ./pico-lua

.PHONY: tests install uninstall clean

all: tests

pico_native.so: ../src/pico.c pico.c
	gcc -Wall -Werror -g -shared -fPIC -DPICO_TESTS -o pico/$@ $^ \
		-llua5.4 -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer -lSDL2_image

test: pico_native.so
	$(TST) tst/$(T).lua

lua: pico_native.so
	$(LUA) ./pico-lua tst/$(T).lua

todo: pico_native.so
	LUA_CPATH="../../pico/?.so;" LUA_PATH="../../?/init.lua;../../?.lua;" ./pico-lua tst/todo/$(T).lua

int: pico_native.so
	$(INT) tst/$(T).lua

tests: pico_native.so
	@echo "Running tests..."
	./pico-lua | grep -q "Usage: pico-lua"
	./pico-lua --help | grep -q "Usage: pico-lua"
	./pico-lua --version | grep -q "v0.4-dev"
	$(TST) tst/anchor.lua
	$(TST) tst/blend_pct.lua
	$(TST) tst/blend_raw.lua
	$(TST) tst/buffer_pct.lua
	$(TST) tst/buffer_raw.lua
	$(TST) tst/clear_alpha.lua
	$(TST) tst/clip_pct.lua
	$(TST) tst/clip_raw.lua
	$(TST) tst/collide_pct.lua
	$(TST) tst/collide_raw.lua
	$(TST) tst/colors.lua
	$(TST) tst/cross.lua
	$(TST) tst/cv.lua
	$(TST) tst/dim.lua
	$(TST) tst/expert.lua
	#$(TST) tst/expert_fps.lua
	$(TST) tst/font.lua
	$(TST) tst/get-set.lua
	$(TST) tst/image_pct.lua
	$(TST) tst/image_raw.lua
	$(TST) tst/init.lua
	$(TST) tst/layer-empty-tile.lua
	$(TST) tst/layer-hier.lua
	$(TST) tst/layers.lua
	$(TST) tst/mouse.lua
	$(TST) tst/mouse-rect-click.lua
	#$(TST) tst/move.lua
	#$(TST) tst/navigate.lua
	$(TST) tst/pixels.lua
	$(TST) tst/pos.lua
	$(TST) tst/quit.lua
	$(TST) tst/rect.lua
	$(TST) tst/rot-flip.lua
	$(TST) tst/set.lua
	$(TST) tst/sheet.lua
	$(TST) tst/shot.lua
	$(TST) tst/size_pct.lua
	$(TST) tst/size_raw.lua
	$(TST) tst/style.lua
	$(TST) tst/text_empty.lua
	$(TST) tst/tile-grid.lua
	$(TST) tst/tiles.lua
	$(TST) tst/video.lua
	$(TST) tst/view-target.lua
	$(TST) tst/view_raw.lua
	$(TST) tst/vs.lua
	@echo ""
	@echo "All tests passed!"

install: pico_native.so
	cp pico-lua /usr/local/bin/
	cp pico/pico_native.so /usr/local/lib/lua/5.4/
	mkdir -p /usr/local/share/lua/5.4/pico/
	cp pico/init.lua /usr/local/share/lua/5.4/pico/

uninstall:
	rm -f /usr/local/bin/pico-lua
	rm -f /usr/local/lib/lua/5.4/pico_native.so
	rm -rf /usr/local/share/lua/5.4/pico/

clean:
	rm -f pico/pico_native.so
