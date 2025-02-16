#!/bin/bash

# Adds Nootka.Music import statement if any QML file uses given class:
# Tmeter, Tclef, Tinstrument, Ttune and this import is missing

# Check if an argument is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <search_string>"
    exit 1
fi

SEARCH_STRING="$1"

# Find files containing the search string but not containing "import Nootka.Music"
files=$(grep -l "$SEARCH_STRING" -R src/*qml | xargs grep -L "import Nootka.Music")

# Loop through each file and replace the line
for file in $files; do
    echo $file
    sed -i '/^import QtQuick 2\.12$/c\import QtQuick\nimport Nootka.Music' "$file"
done
