if ($args.Length -ne 1) {
    Write-Host "Give the filepath where to pack the release"
    exit
}

$GCC_PATH = Read-Host "Absolute path to gcc (mingw64)"
$VSCODE_PATH = Read-Host "Absolute path to vscode"
$VSCODE_EXT_PATH = "$VSCODE_PATH\..\cpptools.vsix"
# SDL libraries
$SDL_PATH = Read-Host "Absolute path to SDL2"
$SDL_IMG_PATH = Read-Host "Absolute path to SDL2_image"
$SDL_TTF_PATH = Read-Host "Absolute path to SDL2_ttf"
$SDL_MIX_PATH = Read-Host "Absolute path to SDL2_mixer"
$SDL_GFX_PATH = Read-Host "Absolute path to SDL2_gfx"

$env:PATH = "$GCC_PATH\bin;$VSCODE_PATH\bin;$env:PATH"

##############################################################################
# SETUP VSCODE

Write-Host ""

if (-not (Test-Path $VSCODE_EXT_PATH)) {
    Write-Host "Downloading vscode cpptools ..."

    Invoke-WebRequest -Uri "https://github.com/microsoft/vscode-cpptools/releases/download/v1.23.6/cpptools-windows-x64.vsix" `
        -OutFile $VSCODE_EXT_PATH
} else {
    Write-Host "vscode cpptools was already downloaded."
}

Write-Host "Done."
Write-Host ""

New-Item -ItemType Directory -Path "$VSCODE_PATH\data" -Force | Out-Null
code --install-extension $VSCODE_EXT_PATH

Write-Host ""

##############################################################################
# BUILDING SDL2_gfx

Write-Host "Building SDL2_gfx ..."

Push-Location -Path $SDL_GFX_PATH
New-Item -ItemType Directory -Path build\bin, build\include\SDL2, build\lib -Force | Out-Null
Set-Location -Path build

gcc -c -fPIC ..\SDL2_framerate.c ..\SDL2_gfxPrimitives.c ..\SDL2_imageFilter.c ..\SDL2_rotozoom.c `
    -I"$SDL_PATH\x86_64-w64-mingw32\include\SDL2"

gcc -shared -o bin\libSDL2_gfx.dll "-Wl,--out-implib,lib\libSDL2_gfx.a" `
    SDL2_framerate.o SDL2_gfxPrimitives.o SDL2_imageFilter.o SDL2_rotozoom.o `
    -L"$SDL_PATH\x86_64-w64-mingw32\bin" -lSDL2

Copy-Item -Path ..\*.h -Destination include\SDL2

Pop-Location

Write-Host "Done."
Write-Host ""

##############################################################################
# BUILDING PICO-SDL

Write-Host "Building pico-sdl ..."

Push-Location -Path ..
New-Item -ItemType Directory -Path build\bin, build\include -Force | Out-Null
Set-Location -Path build

gcc -fPIC -c ..\src\pico.c ..\src\hash.c `
    -I"$SDL_PATH\x86_64-w64-mingw32\include" `
    -I"$SDL_PATH\x86_64-w64-mingw32\include\SDL2" `
    -I"$SDL_IMG_PATH\x86_64-w64-mingw32\include" `
    -I"$SDL_TTF_PATH\x86_64-w64-mingw32\include" `
    -I"$SDL_MIX_PATH\x86_64-w64-mingw32\include" `
    -I"$SDL_GFX_PATH\build\include"
    gcc -shared -o bin\libpico-sdl.dll pico.o hash.o `
    -L"$SDL_PATH\x86_64-w64-mingw32\lib" `
    -L"$SDL_IMG_PATH\x86_64-w64-mingw32\lib" `
    -L"$SDL_TTF_PATH\x86_64-w64-mingw32\lib" `
    -L"$SDL_MIX_PATH\x86_64-w64-mingw32\lib" `
    -L"$SDL_GFX_PATH\build\lib" `
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx

Copy-Item -Path ..\src\*.h -Destination include

Pop-Location

Write-Host "Done."
Write-Host ""

# ##############################################################################
# PACKING RELEASE FILE

Write-Host "Creating release file ..."

Compress-Archive -Path `
    "$SDL_PATH\x86_64-w64-mingw32\include",
    "$SDL_IMG_PATH\x86_64-w64-mingw32\include",
    "$SDL_TTF_PATH\x86_64-w64-mingw32\include",
    "$SDL_MIX_PATH\x86_64-w64-mingw32\include",
    "$SDL_GFX_PATH\build\include",
    ..\build\include,
    "$SDL_PATH\x86_64-w64-mingw32\bin",
    "$SDL_IMG_PATH\x86_64-w64-mingw32\bin",
    "$SDL_TTF_PATH\x86_64-w64-mingw32\bin",
    "$SDL_MIX_PATH\x86_64-w64-mingw32\bin",
    "$SDL_GFX_PATH\build\bin",
    ..\build\bin,
    activate.ps1, activate.bat, $GCC_PATH, $VSCODE_PATH `
    -DestinationPath $args[0]

Write-Host "Done."