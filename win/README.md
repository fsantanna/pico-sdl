# Build the windows release

Open a powershell window.  
**IMPORTANT:** You will need to use it multiple times with changes in environment variables. So keep it open.

## Donwload and extract

Download the required files (Direct download links):
* [gcc (7z)](https://github.com/niXman/mingw-builds-binaries/releases/download/14.2.0-rt_v12-rev2/x86_64-14.2.0-release-win32-seh-ucrt-rt_v12-rev2.7z)
* [SDL2 devel mingw (zip)](https://github.com/libsdl-org/SDL/releases/download/release-2.32.2/SDL2-devel-2.32.2-mingw.zip)
* [SDL2_image devel mingw (zip)](https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.8/SDL2_image-devel-2.8.8-mingw.zip)
* [SDL2_ttf devel mingw (zip)](https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.24.0/SDL2_ttf-devel-2.24.0-mingw.zip)
* [SDL2_mixer devel mingw (zip)](https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.1/SDL2_mixer-devel-2.8.1-mingw.zip)
* [SDL2_gfx source code (zip)](http://www.ferzkopp.net/Software/SDL2_gfx/SDL2_gfx-1.0.4.zip)
* [vscode (zip)](https://code.visualstudio.com/download)
* [vscode cpptools extension (.vsix file)](https://github.com/microsoft/vscode-cpptools/releases/download/v1.23.6/cpptools-windows-x64.vsix)

Extract all the compressed files to a working directory.  
**IMPORTANT:** Extract vscode to a subdirectory and name it *vscode*.

```
cd {working dir}
git clone https://github.com/fsantanna/pico-sdl
```

## Adding gcc to path

You will find a directory named *mingw64* inside where you extracted gcc.  
Add it to path in powershell:
```sh
cd {mingw64 path}
$env:PATH = "$PWD\bin;$env:PATH"
```

## Merging SDL2 libraries

All SDL2 libraries you extracted (except SDL2_gfx) will have a subdirectory named *x86_64-w64-mingw32*.  
Merge all of them into a single *x86_64-w64-mingw32*.

Add it to "path":
```sh
cd {x86_64-w64-mingw32 path}
$env:CPATH = "$PWD\include;$PWD\include\SDL2;$env:CPATH"
$env:LIBRARY_PATH = "$PWD\lib;$env:LIBRARY_PATH"
```

Now compile SDL2_gfx with gcc:
```sh
cd {SDL2_gfx path}
gcc -c -fPIC SDL2_framerate.c SDL2_gfxPrimitives.c SDL2_imageFilter.c SDL2_rotozoom.c
gcc -shared -o libSDL2_gfx.dll "-Wl,--out-implib,libSDL2_gfx.a" SDL2_framerate.o SDL2_gfxPrimitives.o SDL2_imageFilter.o SDL2_rotozoom.o -lSDL2
```

Copy *SDL2_gfx/libSDL2_gfx.dll* to *x86_64-w64-mingw32/bin*

Copy all .h files (5) in *SDL2_gfx* to *x86_64-w64-mingw32/include/SDL2*

Copy *SDL2_gfx/libSDL2_gfx.a* to *x86_64-w64-mingw32/lib*

## Setup vscode

Add downloaded vscode to path:
```sh
cd {vscode path}
$env:PATH = "$PWD\bin;$env:PATH"
```

Toggle portable mode and install cpptools (won't affect existing installs):

```sh
mkdir data
code --install-extension {cpptools.vsix path}
```

## Packing release file

Create a new directory named *pico-sdl-win64-{version}*.

Rename *x86_64-w64-mingw32* to *SDL* and move it to *pico-sdl-win64-{version}*.

Move *vscode* to *pico-sdl-win64-{version}*.

Move *mingw64* to *pico-sdl-win64-{version}*.

Copy *pico-sdl/win/run.bat* to *pico-sdl-win64-{version}*.

Create a directory inside *pico-sdl-win64-{version}* named .vscode.

Copy all .json files inside *pico-sdl/win* to *pico-sdl-win64-{version}/.vscode*.

Copy *pico-sdl/src* to *pico-sdl-win64-{version}*.

Copy *pico-sdl/tiny.ttf* to *pico-sdl-win64-{version}*.

Copy *pico-sdl/tst* to *pico-sdl-win64-{version}*.

Create the .zip *pico-sdl-win64-{version}*.