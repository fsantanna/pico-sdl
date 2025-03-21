# Windows Release

Powershell is required to follow these steps.  
**IMPORTANT:** Don't use a vscode terminal.

```sh
cd win
powershell .\download.ps1 {download-path}
```

`download.ps1` will download files to {download-path} and open {download-path} in explorer when finished.

list of downloads (zips):
* SDL2
* SDL2_image
* SDL2_ttf
* SDL2_mixer
* SDL2_gfx
* gcc
* vscode

Extract each one of these zips.  
**IMPORTANT:** Extract vscode to a subfolder and name it `vscode`.

```sh
powershell .\build.ps1 {release-filepath}
```

`build.ps1` will setup vscode, build SDL2_gfx, build pico-sdl and make {release-filepath}. This script asks you for the root paths of each folder your extracted at runtime.