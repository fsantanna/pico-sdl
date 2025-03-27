@echo off
SET RUN_DIR=%~dp0

SET PATH=%RUN_DIR%SDL\bin;%RUN_DIR%mingw64\bin;%RUN_DIR%vscode\bin;%PATH%
SET CPATH=%RUN_DIR%src;%RUN_DIR%SDL\include;%RUN_DIR%SDL\include\SDL2;%CPATH%
SET LIBRARY_PATH=%RUN_DIR%SDL\lib;%LIBRARY_PATH%

code .
