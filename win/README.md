# Windows Release

You need to be in a powershell terminal.

```sh
cd win
.\download.ps1 {download-path}
```

`download.ps1` will download all required files to {download-path} and open {download-path} in explorer when finished.

list of downloads (zips):
* SDL2
* SDL2_image
* SDL2_ttf
* SDL2_mixer
* SDL2_gfx
* gcc
* vscode

Extract each one of the zips.  
**IMPORTANT:** Extract vscode to a folder and name it `vscode`.

```sh
.\build.ps1 {release-filepath}
```

`build.ps1` will ask you for the paths to all the folders you extracted. It builds SDL2_gfx, pico-sdl and packs everything in {release-filepath}