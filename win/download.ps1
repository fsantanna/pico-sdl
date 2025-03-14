if ($args.Length -ne 1) {
    Write-Host "Give a path where to download the files"
    exit
}

$DOWNLOAD_DIR = $args[0]
$URLS = @(
    "https://github.com/libsdl-org/SDL/releases/download/release-2.32.2/SDL2-devel-2.32.2-mingw.zip",
    "https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.8/SDL2_image-devel-2.8.8-mingw.zip",
    "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.24.0/SDL2_ttf-devel-2.24.0-mingw.zip",
    "https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.1/SDL2_mixer-devel-2.8.1-mingw.zip",
    "https://www.ferzkopp.net/Software/SDL2_gfx/SDL2_gfx-1.0.4.zip",
    "https://github.com/brechtsanders/winlibs_mingw/releases/download/14.2.0posix-12.0.0-ucrt-r3/winlibs-x86_64-posix-seh-gcc-14.2.0-llvm-19.1.7-mingw-w64ucrt-12.0.0-r3.zip",
    "https://vscode.download.prss.microsoft.com/dbazure/download/stable/2fc07b811f760549dab9be9d2bedd06c51dfcb9a/VSCode-win32-x64-1.98.1.zip"
)

if (-not (Test-Path $DOWNLOAD_DIR)) {
    New-Item -ItemType Directory -Path $DOWNLOAD_DIR
}

foreach ($url in $URLS) {
    # Extract the filename from the URL
    $file = [System.IO.Path]::GetFileName($url)
    $downloadPath = Join-Path $DOWNLOAD_DIR $file

    # Check if the file already exists
    if (-not (Test-Path $downloadPath)) {
        Write-Host "Downloading $file ..."
        Invoke-WebRequest -Uri $url -OutFile $downloadPath
        Write-Host "Done."
    } else {
        Write-Host "$file is already downloaded."
    }

    Write-Host ""
}

Invoke-Item -Path $DOWNLOAD_DIR
