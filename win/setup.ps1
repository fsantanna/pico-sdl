$PICO = $PSScriptRoot

$env:PATH = "$PICO;$PICO\SDL\bin;$PICO\mingw64\bin;" + $env:PATH
$env:CPATH = "$PICO\src;$PICO\SDL\include;$PICO\SDL\include\SDL2;" + $env:CPATH
$env:LIBRARY_PATH = "$PICO\SDL\lib;" + $env:LIBRARY_PATH