#!/bin/bash

command_exists() {
    if ! command -v "$1" >/dev/null 2>&1; then
        echo "$1 is not installed. Please install it and try again."
        exit 1
    fi
}

command_exists doxygen

DOXYGEN_LAYOUT_FILE="layout-1.9.xml" doxygen Doxyfile

if command -v zip >/dev/null 2>&1; then
    zip docs.zip -r html/
fi
