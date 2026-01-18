TST = LUA_CPATH="../pico/?.so;" LUA_PATH="../?/init.lua;../?.lua;" xvfb-run -a lua5.4

.PHONY: tests install uninstall clean
.ONESHELL:

pico_native.so: ../src/pico.c pico.c
	gcc -shared -fPIC -DPICO_TESTS -o pico/$@ $^ \
		-llua5.4 -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer -lSDL2_image

tests:
	cd tst/
	$(TST) cv.lua
	$(TST) vs.lua
	$(TST) anchor.lua

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
