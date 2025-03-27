$PICO = $PSScriptRoot

# Get the path and name of the source file
if (-not $args[0]) {
    Write-Host "Usage: .\pico-sdl.ps1 <source_file.c>"
    exit 1
}

$env:PATH = "$PICO\SDL\bin;$PICO\mingw64\bin;" + $env:PATH
$env:CPATH = "$PICO\src;$PICO\SDL\include;$PICO\SDL\include\SDL2;" + $env:CPATH
$env:LIBRARY_PATH = "$PICO\SDL\lib;" + $env:LIBRARY_PATH

$SRC = Resolve-Path $args[0]
$DIR = Split-Path -Parent $SRC
$EXE = [System.IO.Path]::GetFileNameWithoutExtension($SRC)

# Compile the source file
gcc -Wall -o "$DIR\$EXE.exe" "$SRC" `
    "$PICO\src\pico.c" "$PICO\src\dir.c" "$PICO\src\hash.c" `
    -I "$PICO\src" `
    -DSDL_MAIN_HANDLED -lmingw32 -lSDL2main -lSDL2 -lSDL2_gfx -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# Run the compiled executable
Start-Process -NoNewWindow -Wait -FilePath "$DIR\$EXE.exe"
