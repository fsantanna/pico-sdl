TST = LUA_CPATH="../pico/?.so;" LUA_PATH="../?/init.lua;../?.lua;" xvfb-run -a ./pico-lua

.PHONY: tests install uninstall clean

pico_native.so: ../src/pico.c pico.c
	gcc -shared -fPIC -DPICO_TESTS -o pico/$@ $^ \
		-llua5.4 -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer -lSDL2_image

tests:
	$(TST) tst/cv.lua
	$(TST) tst/vs.lua
	$(TST) tst/anchor.lua
	$(TST) tst/blend_raw.lua
	$(TST) tst/blend_pct.lua
	$(TST) tst/buffer_raw.lua
	$(TST) tst/buffer_pct.lua
	$(TST) tst/clip_raw.lua
	$(TST) tst/clip_pct.lua
	$(TST) tst/collide_raw.lua
	$(TST) tst/collide_pct.lua
	$(TST) tst/colors.lua
	$(TST) tst/cross.lua
	$(TST) tst/dim.lua
	$(TST) tst/font.lua
	$(TST) tst/image_raw.lua
	$(TST) tst/image_pct.lua
	#$(TST) tst/mouse.lua
	#$(TST) tst/move.lua
	#$(TST) tst/navigate.lua
	$(TST) tst/pixels.lua
	$(TST) tst/pos.lua
	$(TST) tst/rect.lua

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
