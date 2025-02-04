#!/bin/bash

command_exists() {
    if ! command -v "$1" >/dev/null 2>&1; then
        echo "$1 is not installed. Please install it and try again."
        exit 1
    fi
}

command_exists doxygen

DOXYGEN_LAYOUT_FILE="docs/layout-1.9.xml" doxygen Doxyfile

command_exists zip
zip docs.zip -r docs/html