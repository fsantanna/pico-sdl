# Windows Release

Download the following files:
* [SDL2 devel mingw release](https://github.com/libsdl-org/SDL/releases/release-2.32.2)
* [SDL2_image devel mingw release](https://github.com/libsdl-org/SDL_image/releases/release-2.8.8)
* [SDL2_ttf devel mingw release](https://github.com/libsdl-org/SDL_ttf/releases/release-2.24.0)
* [SDL2_mixer devel mingw release](https://github.com/libsdl-org/SDL_mixer/releases/release-2.8.1)
* [SDL2_gfx source code](https://www.ferzkopp.net/wordpress/2016/01/02/sdl_gfx-sdl2_gfx/)
* [mingw](https://github.com/niXman/mingw-builds-binaries/releases/tag/14.2.0-rt_v12-rev2)
* [vscode zip](https://code.visualstudio.com/download)
* [vscode cpptools extension](https://github.com/microsoft/vscode-cpptools/releases/download/v1.23.6/)

Extract each one of the zips.  
**IMPORTANT:** Extract vscode to a subfolder and name it `vscode`.

```sh
powershell .\build.ps1 {release-filepath}
```

`build.ps1` will setup vscode, build SDL2_gfx, build pico-sdl and make {release-filepath}. This script asks you for the root paths of each folder your extracted at runtime.