#!/bin/bash

command_exists() {
    if ! command -v "$1" >/dev/null 2>&1; then
        echo "$1 is not installed. Please install it and try again."
        exit 1
    fi
}

command_exists doxygen

DOXYGEN_LAYOUT_FILE="layout-1.13.xml" doxygen Doxyfile
