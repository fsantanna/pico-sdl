$RUN_DIR = Split-Path -Parent $MyInvocation.MyCommand.Definition

$env:PATH = "$RUN_DIR\SDL\bin;$RUN_DIR\mingw64\bin;$RUN_DIR\vscode\bin;$RUN_DIR\bin;$env:PATH"
$env:CPATH = "$RUN_DIR\src;$RUN_DIR\SDL\include;$RUN_DIR\SDL\include\SDL2;$env:CPATH"
$env:LIBRARY_PATH = "$RUN_DIR\SDL\lib;$env:LIBRARY_PATH"

code .