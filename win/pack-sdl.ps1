# safety check for correct number of arguments
if ( $args.Length -ne 5) {
    Write-Host "Pass the path to the root dirs of SDL2, SDL2_image, SDL2_ttf, SDL2_mixer and SDL2_gfx!"
    exit
}

$PACK_DIR = "SDL2"

# Create directory if it doesn't exist
if (-not (Test-Path $PACK_DIR)) {
    New-Item -ItemType Directory -Path $PACK_DIR
}

# for each library (except sdl2_gfx)
foreach ($ARG_PATH in $args) {
    $ARG_PATH = "$ARG_PATH\x86_64-w64-mingw32"

    Copy-Item -Path "$ARG_PATH\bin" -Destination $PACK_DIR -Recurse -ErrorAction SilentlyContinue
    Copy-Item -Path "$ARG_PATH\include" -Destination $PACK_DIR -Recurse -ErrorAction SilentlyContinue
    Copy-Item -Path "$ARG_PATH\lib" -Destination $PACK_DIR -Recurse -ErrorAction SilentlyContinue
}

$SDL_GFX_PATH = $args[4]

Copy-Item -Path "$SDL_GFX_PATH\SDL2_gfx.dll" -Destination "$PACK_DIR\bin" -ErrorAction SilentlyContinue
Copy-Item -Path "$SDL_GFX_PATH\*.h" -Destination "$PACK_DIR\include\SDL2" -ErrorAction SilentlyContinue
Copy-Item -Path "$SDL_GFX_PATH\SDL2_gfx.a" -Destination "$PACK_DIR\lib" -ErrorAction SilentlyContinue