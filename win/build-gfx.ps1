# safety check for correct number of arguments
if ( $args.Length -ne 3) {
    Write-Host "Pass the path to the root dirs of gcc, SDL2 and SDL2_gfx!"
    exit
}

$GCC_PATH = $args[0]
$SDL_PATH = $args[1]
$SDL_GFX_PATH = $args[2]

# setting up environment
$env:PATH = "$env:PATH;$GCC_PATH\bin"
$env:CPATH = "$env:CPATH;$SDL_PATH\x86_64-w64-mingw32\include\SDL2"
$env:LIBRARY_PATH = "$env:LIBRARY_PATH;$SDL_PATH\x86_64-w64-mingw32\lib"
$env:LD_LIBRARY_PATH = "$env:LD_LIBRARY_PATH;$SDL_PATH\x86_64-w64-mingw32\lib"

$before = $(Get-Location)
Set-Location -Path $SDL_GFX_PATH

# compile gfx into a shared library
gcc -fPIC -c SDL2_framerate.c SDL2_gfxPrimitives.c SDL2_imageFilter.c SDL2_rotozoom.c
gcc -shared -o SDL2_gfx.dll SDL2_framerate.o SDL2_gfxPrimitives.o SDL2_imageFilter.o SDL2_rotozoom.o -lSDL2
ar rcs SDL2_gfx.a SDL2_framerate.o SDL2_gfxPrimitives.o SDL2_imageFilter.o SDL2_rotozoom.o

Set-Location -Path $before