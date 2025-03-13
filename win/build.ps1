$GCC_PATH = Read-Host "Absolute path to the root dir of gcc (mingw64): "
$VSCODE_PATH = Read-Host "Absolute path to the root dir of vscode: "
$SDL_PATH = Read-Host "Absolute path to the root dir of SDL2: "
$SDL_GFX_PATH = Read-Host "Absolute path to the root dir of SDL2_gfx: "
$PACK_DIR = "release"
$SDL_PACK_DIR = "$PACK_DIR\SDL2"

$LIB_PATHS = @(
    $SDL_PATH
    "SDL2_image"
    "SDL2_ttf"
    "SDL2_mixer"
)
for ($i = 1; $i -lt $LIB_PATHS.Length; $i++) {
    $LIB_PATHS[$i] = Read-Host "Absolute path to the root dir of $($LIB_PATHS[$i]): "
}

##############################################################################
# BUILDING SDL2_gfx

Write-Host "Building SDL2_gfx ..."

# setting up environment
$env:PATH = "$GCC_PATH\bin;$env:PATH"
$env:CPATH = "$SDL_PATH\x86_64-w64-mingw32\include\SDL2;$env:CPATH"
$env:LIBRARY_PATH = "$SDL_PATH\x86_64-w64-mingw32\lib;$env:LIBRARY_PATH"
$env:LD_LIBRARY_PATH = "$SDL_PATH\x86_64-w64-mingw32\bin;$env:LD_LIBRARY_PATH"

# save working directory and go to sdl2_gfx location
$before = $(Get-Location)
Set-Location -Path $SDL_GFX_PATH

# compile gfx into a shared library
gcc -fPIC -c SDL2_framerate.c SDL2_gfxPrimitives.c SDL2_imageFilter.c SDL2_rotozoom.c
gcc -shared -o SDL2_gfx.dll SDL2_framerate.o SDL2_gfxPrimitives.o SDL2_imageFilter.o SDL2_rotozoom.o -lSDL2
ar rcs SDL2_gfx.a SDL2_framerate.o SDL2_gfxPrimitives.o SDL2_imageFilter.o SDL2_rotozoom.o

# go back to working directory
Set-Location -Path $before

Write-Host "Done."
Write-Host ""

##############################################################################
# PACKING SDL LIBRARIES

Write-Host "Packing SDL2 libraries ..."

# Create directory if it doesn't exist
if (-not (Test-Path $SDL_PACK_DIR)) {
    New-Item -ItemType Directory -Path $SDL_PACK_DIR
}

# for each library (except SDL2_gfx)
foreach ($lib_path in $LIB_PATHS) {
    $lib_path = "$lib_path\x86_64-w64-mingw32"

    Copy-Item -Path "$lib_path\bin" -Destination $SDL_PACK_DIR -Recurse -ErrorAction SilentlyContinue
    Copy-Item -Path "$lib_path\include" -Destination $SDL_PACK_DIR -Recurse -ErrorAction SilentlyContinue
    Copy-Item -Path "$lib_path\lib" -Destination $SDL_PACK_DIR -Recurse -ErrorAction SilentlyContinue
}

# pack SDL2_gfx
Copy-Item -Path "$SDL_GFX_PATH\SDL2_gfx.dll" -Destination "$SDL_PACK_DIR\bin" -ErrorAction SilentlyContinue
Copy-Item -Path "$SDL_GFX_PATH\*.h" -Destination "$SDL_PACK_DIR\include\SDL2" -ErrorAction SilentlyContinue
Copy-Item -Path "$SDL_GFX_PATH\SDL2_gfx.a" -Destination "$SDL_PACK_DIR\lib" -ErrorAction SilentlyContinue

Write-Host "Done."
Write-Host ""

##############################################################################
# MOVING GCC AND VSCODE

Write-Host "Moving gcc and vscode to release folder ..."

Move-Item -Path "$GCC_PATH" -Destination "$PACK_DIR\gcc" -ErrorAction SilentlyContinue
Move-Item -Path "$VSCODE_PATH" -Destination "$PACK_DIR\vscode" -ErrorAction SilentlyContinue

Write-Host "Done."
Write-Host ""