@echo off
rem Get the directory of the batch script
set "WORKING_DIR=%~dp0"

rem Modify environment variables
set "PATH=%WORKING_DIR%\gcc\bin;%WORKING_DIR%\vscode\bin;%WORKING_DIR%\bin;%PATH%"
set "CPATH=%WORKING_DIR%\include;%CPATH%"
set "LIBRARY_PATH=%WORKING_DIR%;%LIBRARY_PATH%"
set "LD_LIBRARY_PATH=%WORKING_DIR%;%LD_LIBRARY_PATH%"

echo "Pico IDE activated!"