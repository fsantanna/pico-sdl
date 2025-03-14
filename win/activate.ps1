$WORKING_DIR = Split-Path -Parent $MyInvocation.MyCommand.Definition

$env:PATH = "$WORKING_DIR\mingw64\bin;$WORKING_DIR\vscode\bin;$WORKING_DIR\bin;$env:PATH"
$env:CPATH = "$WORKING_DIR\include;$env:CPATH"
$env:LIBRARY_PATH = "$WORKING_DIR\bin;$env:LIBRARY_PATH"
$env:LD_LIBRARY_PATH = "$WORKING_DIR\bin;$env:LD_LIBRARY_PATH"

Write-Host "Pico IDE activated!"